#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "agc_fixed_runtime.h"

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
    uint64_t peak_protector_frames;
    uint64_t limiter_frames;
    float input_peak;
    float output_peak;
    float input_rms;
    float output_rms;
    float max_applied_gain;
} RunSummary;

static uint16_t read_u16_le(FILE *fp) {
    uint8_t b[2];
    fread(b, 1, 2, fp);
    return (uint16_t)(b[0] | ((uint16_t)b[1] << 8));
}

static uint32_t read_u32_le(FILE *fp) {
    uint8_t b[4];
    fread(b, 1, 4, fp);
    return (uint32_t)(b[0] |
                     ((uint32_t)b[1] << 8) |
                     ((uint32_t)b[2] << 16) |
                     ((uint32_t)b[3] << 24));
}

static void write_u16_le(FILE *fp, uint16_t v) {
    uint8_t b[2];
    b[0] = (uint8_t)(v & 0xff);
    b[1] = (uint8_t)((v >> 8) & 0xff);
    fwrite(b, 1, 2, fp);
}

static void write_u32_le(FILE *fp, uint32_t v) {
    uint8_t b[4];
    b[0] = (uint8_t)(v & 0xff);
    b[1] = (uint8_t)((v >> 8) & 0xff);
    b[2] = (uint8_t)((v >> 16) & 0xff);
    b[3] = (uint8_t)((v >> 24) & 0xff);
    fwrite(b, 1, 4, fp);
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
            if (sub_size > 16) {
                fseek(fp, (long)(sub_size - 16), SEEK_CUR);
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

static int wav_write_header(FILE *fp, const WavInfo *info, uint32_t sample_count) {
    uint32_t data_bytes = sample_count * sizeof(int16_t);
    uint32_t riff_size = 36U + data_bytes;

    fwrite("RIFF", 1, 4, fp);
    write_u32_le(fp, riff_size);
    fwrite("WAVE", 1, 4, fp);
    fwrite("fmt ", 1, 4, fp);
    write_u32_le(fp, 16);
    write_u16_le(fp, 1);
    write_u16_le(fp, 1);
    write_u32_le(fp, (uint32_t)info->sample_rate_hz);
    write_u32_le(fp, (uint32_t)(info->sample_rate_hz * 2));
    write_u16_le(fp, 2);
    write_u16_le(fp, 16);
    fwrite("data", 1, 4, fp);
    write_u32_le(fp, data_bytes);
    return 1;
}

static float s16_to_float(int16_t x) {
    return x >= 0 ? ((float)x / 32767.0f) : ((float)x / 32768.0f);
}

static void update_signal_stats(RunSummary *summary,
                                const int16_t *input,
                                const int16_t *output,
                                int16_t sample_count,
                                double *input_energy,
                                double *output_energy,
                                uint64_t *total_samples) {
    int16_t i;
    for (i = 0; i < sample_count; ++i) {
        float in_f = s16_to_float(input[i]);
        float out_f = s16_to_float(output[i]);
        float in_abs = in_f >= 0 ? in_f : -in_f;
        float out_abs = out_f >= 0 ? out_f : -out_f;

        if (in_abs > summary->input_peak) {
            summary->input_peak = in_abs;
        }
        if (out_abs > summary->output_peak) {
            summary->output_peak = out_abs;
        }
        *input_energy += (double)in_f * (double)in_f;
        *output_energy += (double)out_f * (double)out_f;
        (*total_samples)++;
    }
}

int main(int argc, char **argv) {
    FILE *in_fp;
    FILE *out_fp;
    WavInfo wav_info;
    AgcFixedRuntime runtime;
    RunSummary summary;
    AgcFixedFrameMetrics metrics;
    int16_t *input_samples = NULL;
    int16_t *output_samples = NULL;
    int16_t *frame_in = NULL;
    int16_t *frame_out = NULL;
    uint32_t sample_count;
    int16_t frame_samples;
    uint32_t frame_start;
    double input_energy = 0.0;
    double output_energy = 0.0;
    uint64_t total_samples = 0;
    uint16_t target_rms_percent = 80;
    uint16_t max_gain_db = 18;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input.wav> <output.wav> [target_rms_percent] [max_gain_db]\n", argv[0]);
        return 1;
    }
    if (argc >= 4) {
        target_rms_percent = (uint16_t)atoi(argv[3]);
    }
    if (argc >= 5) {
        max_gain_db = (uint16_t)atoi(argv[4]);
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

    sample_count = wav_info.data_size / sizeof(int16_t);
    input_samples = (int16_t *)calloc(sample_count, sizeof(int16_t));
    output_samples = (int16_t *)calloc(sample_count, sizeof(int16_t));
    if (input_samples == NULL || output_samples == NULL) {
        fprintf(stderr, "Allocation failed.\n");
        fclose(in_fp);
        free(input_samples);
        free(output_samples);
        return 1;
    }

    fseek(in_fp, (long)wav_info.data_offset, SEEK_SET);
    if (fread(input_samples, sizeof(int16_t), sample_count, in_fp) != sample_count) {
        fprintf(stderr, "Failed to read PCM data.\n");
        fclose(in_fp);
        free(input_samples);
        free(output_samples);
        return 1;
    }
    fclose(in_fp);

    agc_fixed_runtime_init(&runtime, wav_info.sample_rate_hz, target_rms_percent, max_gain_db);
    frame_samples = agc_fixed_runtime_frame_samples(&runtime);
    frame_in = (int16_t *)calloc((size_t)frame_samples, sizeof(int16_t));
    frame_out = (int16_t *)calloc((size_t)frame_samples, sizeof(int16_t));
    if (frame_in == NULL || frame_out == NULL) {
        fprintf(stderr, "Frame allocation failed.\n");
        free(input_samples);
        free(output_samples);
        free(frame_in);
        free(frame_out);
        return 1;
    }

    memset(&summary, 0, sizeof(summary));
    memset(&metrics, 0, sizeof(metrics));

    for (frame_start = 0; frame_start < sample_count; frame_start += (uint32_t)frame_samples) {
        uint32_t remaining = sample_count - frame_start;
        uint32_t current = remaining < (uint32_t)frame_samples ? remaining : (uint32_t)frame_samples;

        memset(frame_in, 0, (size_t)frame_samples * sizeof(int16_t));
        memcpy(frame_in, &input_samples[frame_start], current * sizeof(int16_t));

        if (!agc_fixed_runtime_process(&runtime, frame_in, frame_out, &metrics)) {
            fprintf(stderr, "AGC process failed.\n");
            free(input_samples);
            free(output_samples);
            free(frame_in);
            free(frame_out);
            return 1;
        }

        memcpy(&output_samples[frame_start], frame_out, current * sizeof(int16_t));

        summary.frames++;
        if (metrics.gate_open) {
            summary.gate_frames++;
        }
        if (metrics.peak_protector_active) {
            summary.peak_protector_frames++;
        }
        if (metrics.limiter_active) {
            summary.limiter_frames++;
        }
        {
            float applied_gain = (float)metrics.applied_gain_q20 / (float)(1 << 20);
            if (applied_gain > summary.max_applied_gain) {
                summary.max_applied_gain = applied_gain;
            }
        }

        update_signal_stats(&summary,
                            &input_samples[frame_start],
                            &output_samples[frame_start],
                            (int16_t)current,
                            &input_energy,
                            &output_energy,
                            &total_samples);
    }

    summary.input_rms = total_samples > 0 ? (float)sqrt(input_energy / (double)total_samples) : 0.0f;
    summary.output_rms = total_samples > 0 ? (float)sqrt(output_energy / (double)total_samples) : 0.0f;

    out_fp = fopen(argv[2], "wb");
    if (out_fp == NULL) {
        fprintf(stderr, "Failed to open output WAV: %s\n", argv[2]);
        free(input_samples);
        free(output_samples);
        free(frame_in);
        free(frame_out);
        return 1;
    }
    wav_write_header(out_fp, &wav_info, sample_count);
    fwrite(output_samples, sizeof(int16_t), sample_count, out_fp);
    fclose(out_fp);

    printf("%s\n", argv[1]);
    printf("  sample_rate=%ld samples=%lu frames=%llu frame_samples=%d\n",
           (long)wav_info.sample_rate_hz,
           (unsigned long)sample_count,
           (unsigned long long)summary.frames,
           frame_samples);
    printf("  input_peak=%.4f input_rms=%.4f\n", summary.input_peak, summary.input_rms);
    printf("  output_peak=%.4f output_rms=%.4f\n", summary.output_peak, summary.output_rms);
    printf("  gate_frames=%llu peak_protector_frames=%llu limiter_frames=%llu max_gain=%.4f\n",
           (unsigned long long)summary.gate_frames,
           (unsigned long long)summary.peak_protector_frames,
           (unsigned long long)summary.limiter_frames,
           summary.max_applied_gain);

    free(input_samples);
    free(output_samples);
    free(frame_in);
    free(frame_out);
    return 0;
}
