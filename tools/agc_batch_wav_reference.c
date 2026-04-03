#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "agc_config.h"
#include "agc_pipeline.h"
#include "agc_wav.h"

typedef struct WavInfo {
    int32_t sample_rate_hz;
    uint16_t bits_per_sample;
    uint16_t channels;
    uint32_t data_offset;
    uint32_t data_size;
} WavInfo;

typedef struct RunSummary {
    uint64_t frames;
    uint64_t gate_frames;
    uint64_t headroom_limited_frames;
    uint64_t compressor_frames;
    uint64_t peak_protector_frames;
    uint64_t limiter_frames;
    float input_peak;
    float output_peak;
    float input_rms;
    float output_rms;
    float max_applied_gain;
    float max_compressor_gain_reduction_db;
} RunSummary;

static uint16_t read_u16_le(FILE *fp) {
    uint8_t b[2];
    if (fread(b, 1, 2, fp) != 2) {
        return 0;
    }
    return (uint16_t)(b[0] | ((uint16_t)b[1] << 8));
}

static uint32_t read_u32_le(FILE *fp) {
    uint8_t b[4];
    if (fread(b, 1, 4, fp) != 4) {
        return 0;
    }
    return (uint32_t)(b[0] |
                      ((uint32_t)b[1] << 8) |
                      ((uint32_t)b[2] << 16) |
                      ((uint32_t)b[3] << 24));
}

static int wav_read_info(FILE *fp, WavInfo *info) {
    char chunk_id[4];
    uint32_t chunk_size;
    char wave_id[4];

    memset(info, 0, sizeof(*info));
    if (fread(chunk_id, 1, 4, fp) != 4) {
        return 0;
    }
    chunk_size = read_u32_le(fp);
    (void)chunk_size;
    if (fread(wave_id, 1, 4, fp) != 4) {
        return 0;
    }
    if (memcmp(chunk_id, "RIFF", 4) != 0 || memcmp(wave_id, "WAVE", 4) != 0) {
        return 0;
    }

    while (!feof(fp)) {
        char sub_id[4];
        uint32_t sub_size;

        if (fread(sub_id, 1, 4, fp) != 4) {
            break;
        }
        sub_size = read_u32_le(fp);

        if (memcmp(sub_id, "fmt ", 4) == 0) {
            uint16_t audio_format = read_u16_le(fp);
            info->channels = read_u16_le(fp);
            info->sample_rate_hz = (int32_t)read_u32_le(fp);
            (void)read_u32_le(fp);
            (void)read_u16_le(fp);
            info->bits_per_sample = read_u16_le(fp);
            if (audio_format != 1) {
                return 0;
            }
            if (sub_size > 16U) {
                fseek(fp, (long)(sub_size - 16U), SEEK_CUR);
            }
        } else if (memcmp(sub_id, "data", 4) == 0) {
            info->data_offset = (uint32_t)ftell(fp);
            info->data_size = sub_size;
            fseek(fp, (long)sub_size, SEEK_CUR);
        } else {
            fseek(fp, (long)sub_size, SEEK_CUR);
        }

        if ((sub_size & 1U) != 0U) {
            fseek(fp, 1, SEEK_CUR);
        }
    }

    return info->channels == 1 &&
           info->bits_per_sample == 16 &&
           info->sample_rate_hz > 0 &&
           info->data_size > 0;
}

static float s16_to_float(int16_t x) {
    return x >= 0 ? ((float)x / 32767.0f) : ((float)x / 32768.0f);
}

static float db_to_linear(float db) {
    return powf(10.0f, db / 20.0f);
}

static int is_mode_string(const char *value, const char *expected) {
    while (*value != '\0' && *expected != '\0') {
        char a = *value;
        char b = *expected;

        if (a >= 'A' && a <= 'Z') {
            a = (char)(a - 'A' + 'a');
        }
        if (b >= 'A' && b <= 'Z') {
            b = (char)(b - 'A' + 'a');
        }
        if (a != b) {
            return 0;
        }
        ++value;
        ++expected;
    }
    return *value == '\0' && *expected == '\0';
}

static void update_signal_stats(RunSummary *summary,
                                const float *input,
                                const float *output,
                                size_t sample_count,
                                double *input_energy,
                                double *output_energy,
                                uint64_t *total_samples) {
    size_t i;
    for (i = 0; i < sample_count; ++i) {
        float in_abs = fabsf(input[i]);
        float out_abs = fabsf(output[i]);

        if (in_abs > summary->input_peak) {
            summary->input_peak = in_abs;
        }
        if (out_abs > summary->output_peak) {
            summary->output_peak = out_abs;
        }

        *input_energy += (double)input[i] * (double)input[i];
        *output_energy += (double)output[i] * (double)output[i];
        (*total_samples)++;
    }
}

int main(int argc, char **argv) {
    FILE *in_fp;
    WavInfo wav_info;
    AgcConfig config;
    PipelineState state;
    RunSummary summary;
    FrameMetrics metrics;
    int16_t *input_pcm = NULL;
    float *input_samples = NULL;
    float *output_samples = NULL;
    float *frame_in = NULL;
    float *frame_out = NULL;
    uint32_t sample_count;
    size_t frame_start;
    double input_energy = 0.0;
    double output_energy = 0.0;
    uint64_t total_samples = 0;
    uint16_t target_rms_percent = 80;
    uint16_t max_gain_db = 18;
    uint16_t gate_threshold_percent = 5;
    AgcMode mode = AGC_MODE_AM;

    if (argc < 3) {
        fprintf(stderr,
                "Usage: %s <input.wav> <output.wav> [target_rms_percent] [max_gain_db] [gate_threshold_percent] [mode]\n",
                argv[0]);
        return 1;
    }
    if (argc >= 4) {
        target_rms_percent = (uint16_t)atoi(argv[3]);
    }
    if (argc >= 5) {
        max_gain_db = (uint16_t)atoi(argv[4]);
    }
    if (argc >= 6) {
        gate_threshold_percent = (uint16_t)atoi(argv[5]);
    }
    if (argc >= 7) {
        if (is_mode_string(argv[6], "digital")) {
            mode = AGC_MODE_DIGITAL;
        } else {
            mode = AGC_MODE_AM;
        }
    }

    in_fp = fopen(argv[1], "rb");
    if (in_fp == NULL) {
        fprintf(stderr, "Failed to open input WAV: %s\n", argv[1]);
        return 1;
    }
    if (!wav_read_info(in_fp, &wav_info)) {
        fprintf(stderr, "Unsupported WAV format in %s (need mono PCM16).\n", argv[1]);
        fclose(in_fp);
        return 1;
    }

    sample_count = wav_info.data_size / (uint32_t)sizeof(int16_t);
    input_pcm = (int16_t *)calloc(sample_count, sizeof(int16_t));
    input_samples = (float *)calloc(sample_count, sizeof(float));
    output_samples = (float *)calloc(sample_count, sizeof(float));
    if (input_pcm == NULL || input_samples == NULL || output_samples == NULL) {
        fprintf(stderr, "Allocation failed.\n");
        fclose(in_fp);
        free(input_pcm);
        free(input_samples);
        free(output_samples);
        return 1;
    }

    fseek(in_fp, (long)wav_info.data_offset, SEEK_SET);
    if (fread(input_pcm, sizeof(int16_t), sample_count, in_fp) != sample_count) {
        fprintf(stderr, "Failed to read PCM data.\n");
        fclose(in_fp);
        free(input_pcm);
        free(input_samples);
        free(output_samples);
        return 1;
    }
    fclose(in_fp);

    {
        uint32_t i;
        for (i = 0; i < sample_count; ++i) {
            input_samples[i] = s16_to_float(input_pcm[i]);
        }
    }

    config = agc_config_preset(wav_info.sample_rate_hz, mode);
    config.target_rms_fs = (float)target_rms_percent / 100.0f;
    config.max_gain = db_to_linear((float)max_gain_db);
    config.gate_threshold = (float)gate_threshold_percent / 100.0f;

    agc_pipeline_init(&state);

    frame_in = (float *)calloc((size_t)config.frame_samples, sizeof(float));
    frame_out = (float *)calloc((size_t)config.frame_samples, sizeof(float));
    if (frame_in == NULL || frame_out == NULL) {
        fprintf(stderr, "Frame allocation failed.\n");
        free(input_pcm);
        free(input_samples);
        free(output_samples);
        free(frame_in);
        free(frame_out);
        return 1;
    }

    memset(&summary, 0, sizeof(summary));
    memset(&metrics, 0, sizeof(metrics));

    for (frame_start = 0; frame_start < (size_t)sample_count; frame_start += (size_t)config.frame_samples) {
        size_t remaining = (size_t)sample_count - frame_start;
        size_t current = remaining < (size_t)config.frame_samples ? remaining : (size_t)config.frame_samples;

        memset(frame_in, 0, (size_t)config.frame_samples * sizeof(float));
        memcpy(frame_in, &input_samples[frame_start], current * sizeof(float));

        agc_process_frame(frame_in,
                          frame_out,
                          (size_t)config.frame_samples,
                          &state,
                          &config,
                          &metrics);

        memcpy(&output_samples[frame_start], frame_out, current * sizeof(float));

        summary.frames++;
        if (metrics.gate_open) {
            summary.gate_frames++;
        }
        if (metrics.headroom_limited) {
            summary.headroom_limited_frames++;
        }
        if (metrics.compressor_active) {
            summary.compressor_frames++;
        }
        if (metrics.peak_protector_active) {
            summary.peak_protector_frames++;
        }
        if (metrics.limiter_active) {
            summary.limiter_frames++;
        }
        if (metrics.applied_gain > summary.max_applied_gain) {
            summary.max_applied_gain = metrics.applied_gain;
        }
        if (metrics.compressor_gain_reduction_db > summary.max_compressor_gain_reduction_db) {
            summary.max_compressor_gain_reduction_db = metrics.compressor_gain_reduction_db;
        }

        update_signal_stats(&summary,
                            &input_samples[frame_start],
                            &output_samples[frame_start],
                            current,
                            &input_energy,
                            &output_energy,
                            &total_samples);
    }

    summary.input_rms = total_samples > 0 ? (float)sqrt(input_energy / (double)total_samples) : 0.0f;
    summary.output_rms = total_samples > 0 ? (float)sqrt(output_energy / (double)total_samples) : 0.0f;

    if (!agc_write_wav_mono_s16(argv[2], output_samples, sample_count, wav_info.sample_rate_hz)) {
        fprintf(stderr, "Failed to write output WAV: %s\n", argv[2]);
        free(input_pcm);
        free(input_samples);
        free(output_samples);
        free(frame_in);
        free(frame_out);
        return 1;
    }

    printf("%s\n", argv[1]);
    printf("  mode=%s\n", agc_mode_name(config.mode));
    printf("  sample_rate=%ld samples=%lu frames=%llu frame_samples=%ld\n",
           (long)wav_info.sample_rate_hz,
           (unsigned long)sample_count,
           (unsigned long long)summary.frames,
           (long)config.frame_samples);
    printf("  input_peak=%.4f input_rms=%.4f\n", summary.input_peak, summary.input_rms);
    printf("  output_peak=%.4f output_rms=%.4f\n", summary.output_peak, summary.output_rms);
    printf("  gate_frames=%llu headroom_limited_frames=%llu compressor_frames=%llu peak_protector_frames=%llu limiter_frames=%llu\n",
           (unsigned long long)summary.gate_frames,
           (unsigned long long)summary.headroom_limited_frames,
           (unsigned long long)summary.compressor_frames,
           (unsigned long long)summary.peak_protector_frames,
           (unsigned long long)summary.limiter_frames);
    printf("  max_gain=%.4f max_compressor_gain_reduction_db=%.2f\n",
           summary.max_applied_gain,
           summary.max_compressor_gain_reduction_db);

    free(input_pcm);
    free(input_samples);
    free(output_samples);
    free(frame_in);
    free(frame_out);
    return 0;
}
