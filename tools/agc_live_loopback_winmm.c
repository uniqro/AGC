#include <conio.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <mmsystem.h>

#include "agc_fixed_runtime.h"

#pragma comment(lib, "winmm.lib")

enum {
    AGC_LIVE_BUFFER_COUNT = 4
};

typedef struct LiveWavWriter {
    FILE *fp;
    uint32_t data_bytes;
    int32_t sample_rate_hz;
} LiveWavWriter;

typedef struct LiveIoContext {
    HWAVEIN wave_in;
    HWAVEOUT wave_out;
    UINT input_device_id;
    UINT output_device_id;
    AgcFixedRuntime runtime;
    int16_t input_buffers[AGC_LIVE_BUFFER_COUNT][AGC_FIXED_MAX_FRAME_SAMPLES];
    int16_t output_buffers[AGC_LIVE_BUFFER_COUNT][AGC_FIXED_MAX_FRAME_SAMPLES];
    WAVEHDR input_headers[AGC_LIVE_BUFFER_COUNT];
    WAVEHDR output_headers[AGC_LIVE_BUFFER_COUNT];
    LiveWavWriter input_writer;
    LiveWavWriter output_writer;
    int16_t frame_samples;
    int32_t sample_rate_hz;
    uint32_t processed_frames;
    int meter_enabled;
    int record_enabled;
} LiveIoContext;

static void agc_write_wav_header(FILE *fp, uint32_t data_bytes, int32_t sample_rate_hz) {
    uint32_t riff_size = 36U + data_bytes;
    uint16_t audio_format = 1;
    uint16_t num_channels = 1;
    uint16_t bits_per_sample = 16;
    uint32_t byte_rate = (uint32_t)(sample_rate_hz * num_channels * (bits_per_sample / 8U));
    uint16_t block_align = (uint16_t)(num_channels * (bits_per_sample / 8U));
    uint32_t fmt_size = 16U;

    fseek(fp, 0, SEEK_SET);
    fwrite("RIFF", 1, 4, fp);
    fwrite(&riff_size, sizeof(riff_size), 1, fp);
    fwrite("WAVE", 1, 4, fp);
    fwrite("fmt ", 1, 4, fp);
    fwrite(&fmt_size, sizeof(fmt_size), 1, fp);
    fwrite(&audio_format, sizeof(audio_format), 1, fp);
    fwrite(&num_channels, sizeof(num_channels), 1, fp);
    fwrite(&sample_rate_hz, sizeof(sample_rate_hz), 1, fp);
    fwrite(&byte_rate, sizeof(byte_rate), 1, fp);
    fwrite(&block_align, sizeof(block_align), 1, fp);
    fwrite(&bits_per_sample, sizeof(bits_per_sample), 1, fp);
    fwrite("data", 1, 4, fp);
    fwrite(&data_bytes, sizeof(data_bytes), 1, fp);
}

static int agc_live_wav_open(LiveWavWriter *writer, const char *path, int32_t sample_rate_hz) {
    memset(writer, 0, sizeof(*writer));
    writer->fp = fopen(path, "wb");
    if (writer->fp == NULL) {
        return 0;
    }
    writer->sample_rate_hz = sample_rate_hz;
    agc_write_wav_header(writer->fp, 0, sample_rate_hz);
    return 1;
}

static void agc_live_wav_append(LiveWavWriter *writer, const int16_t *samples, int16_t sample_count) {
    uint32_t bytes;

    if (writer->fp == NULL || sample_count <= 0) {
        return;
    }
    bytes = (uint32_t)(sample_count * (int16_t)sizeof(samples[0]));
    fwrite(samples, sizeof(samples[0]), (size_t)sample_count, writer->fp);
    writer->data_bytes += bytes;
}

static void agc_live_wav_close(LiveWavWriter *writer) {
    if (writer->fp == NULL) {
        return;
    }
    agc_write_wav_header(writer->fp, writer->data_bytes, writer->sample_rate_hz);
    fclose(writer->fp);
    writer->fp = NULL;
}

static int16_t agc_live_peak_s16(const int16_t *samples, int16_t sample_count) {
    int16_t peak = 0;
    int16_t i;

    for (i = 0; i < sample_count; ++i) {
        int16_t x = samples[i];
        int16_t abs_x;
        if (x == -32768) {
            abs_x = 32767;
        } else {
            abs_x = (x < 0) ? (int16_t)(-x) : x;
        }
        if (abs_x > peak) {
            peak = abs_x;
        }
    }
    return peak;
}

static void agc_live_print_bar(const char *label, int16_t peak_q15) {
    int width = 24;
    int filled = ((int32_t)peak_q15 * width) / 32767;
    int i;

    printf("%s[", label);
    for (i = 0; i < width; ++i) {
        putchar(i < filled ? '#' : '.');
    }
    printf("] %3d%% ", (int)(((int32_t)peak_q15 * 100) / 32767));
}

static void agc_live_print_meter(LiveIoContext *ctx,
                                 const int16_t *input,
                                 const int16_t *output) {
    int16_t in_peak = agc_live_peak_s16(input, ctx->frame_samples);
    int16_t out_peak = agc_live_peak_s16(output, ctx->frame_samples);

    if (!ctx->meter_enabled) {
        return;
    }

    printf("\r");
    agc_live_print_bar("IN ", in_peak);
    agc_live_print_bar("OUT ", out_peak);
    printf("frames=%lu", (unsigned long)ctx->processed_frames);
    fflush(stdout);
}

static void agc_live_list_devices(void) {
    UINT i;
    UINT in_count = waveInGetNumDevs();
    UINT out_count = waveOutGetNumDevs();

    printf("Input devices:\n");
    for (i = 0; i < in_count; ++i) {
        WAVEINCAPS caps;
        if (waveInGetDevCaps(i, &caps, sizeof(caps)) == MMSYSERR_NOERROR) {
            printf("  %u: %s\n", i, caps.szPname);
        }
    }

    printf("Output devices:\n");
    for (i = 0; i < out_count; ++i) {
        WAVEOUTCAPS caps;
        if (waveOutGetDevCaps(i, &caps, sizeof(caps)) == MMSYSERR_NOERROR) {
            printf("  %u: %s\n", i, caps.szPname);
        }
    }
}

static void agc_live_print_usage(const char *exe_name) {
    printf("Usage:\n");
    printf("  %s --list\n", exe_name);
    printf("  %s [sample_rate] [target_rms_percent] [max_gain_db] [input_dev] [output_dev] [record_prefix]\n",
           exe_name);
    printf("\n");
    printf("Examples:\n");
    printf("  %s\n", exe_name);
    printf("  %s 16000 42 12\n", exe_name);
    printf("  %s 16000 42 12 1 3 session01\n", exe_name);
}

static int agc_live_wait_for_output_slot(LiveIoContext *ctx, int buffer_index) {
    while ((ctx->output_headers[buffer_index].dwFlags & WHDR_INQUEUE) != 0) {
        Sleep(1);
        if (_kbhit()) {
            return 0;
        }
    }
    return 1;
}

static int agc_live_open_devices(LiveIoContext *ctx) {
    WAVEFORMATEX format;
    MMRESULT mmr;
    int i;

    memset(&format, 0, sizeof(format));
    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = 1;
    format.nSamplesPerSec = (DWORD)ctx->sample_rate_hz;
    format.wBitsPerSample = 16;
    format.nBlockAlign = (WORD)(format.nChannels * (format.wBitsPerSample / 8));
    format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;

    mmr = waveInOpen(&ctx->wave_in, ctx->input_device_id, &format, 0, 0, CALLBACK_NULL);
    if (mmr != MMSYSERR_NOERROR) {
        return 0;
    }

    mmr = waveOutOpen(&ctx->wave_out, ctx->output_device_id, &format, 0, 0, CALLBACK_NULL);
    if (mmr != MMSYSERR_NOERROR) {
        waveInClose(ctx->wave_in);
        ctx->wave_in = 0;
        return 0;
    }

    for (i = 0; i < AGC_LIVE_BUFFER_COUNT; ++i) {
        memset(&ctx->input_headers[i], 0, sizeof(ctx->input_headers[i]));
        ctx->input_headers[i].lpData = (LPSTR)ctx->input_buffers[i];
        ctx->input_headers[i].dwBufferLength =
            (DWORD)(ctx->frame_samples * (int16_t)sizeof(ctx->input_buffers[i][0]));

        mmr = waveInPrepareHeader(ctx->wave_in, &ctx->input_headers[i], sizeof(ctx->input_headers[i]));
        if (mmr != MMSYSERR_NOERROR) {
            return 0;
        }

        mmr = waveInAddBuffer(ctx->wave_in, &ctx->input_headers[i], sizeof(ctx->input_headers[i]));
        if (mmr != MMSYSERR_NOERROR) {
            return 0;
        }

        memset(&ctx->output_headers[i], 0, sizeof(ctx->output_headers[i]));
        ctx->output_headers[i].lpData = (LPSTR)ctx->output_buffers[i];
        ctx->output_headers[i].dwBufferLength =
            (DWORD)(ctx->frame_samples * (int16_t)sizeof(ctx->output_buffers[i][0]));

        mmr = waveOutPrepareHeader(ctx->wave_out, &ctx->output_headers[i], sizeof(ctx->output_headers[i]));
        if (mmr != MMSYSERR_NOERROR) {
            return 0;
        }
    }

    return 1;
}

static void agc_live_close_devices(LiveIoContext *ctx) {
    int i;

    if (ctx->wave_in != 0) {
        waveInStop(ctx->wave_in);
        waveInReset(ctx->wave_in);
    }
    if (ctx->wave_out != 0) {
        waveOutReset(ctx->wave_out);
    }

    for (i = 0; i < AGC_LIVE_BUFFER_COUNT; ++i) {
        if (ctx->wave_in != 0) {
            waveInUnprepareHeader(ctx->wave_in, &ctx->input_headers[i], sizeof(ctx->input_headers[i]));
        }
        if (ctx->wave_out != 0) {
            waveOutUnprepareHeader(ctx->wave_out, &ctx->output_headers[i], sizeof(ctx->output_headers[i]));
        }
    }

    if (ctx->wave_in != 0) {
        waveInClose(ctx->wave_in);
        ctx->wave_in = 0;
    }
    if (ctx->wave_out != 0) {
        waveOutClose(ctx->wave_out);
        ctx->wave_out = 0;
    }
}

int main(int argc, char **argv) {
    LiveIoContext ctx;
    MMRESULT mmr;
    uint16_t target_rms_percent = 42;
    uint16_t max_gain_db = 12;
    const char *record_prefix = 0;
    int i;
    int running = 1;

    if (argc >= 2 && strcmp(argv[1], "--list") == 0) {
        agc_live_list_devices();
        return 0;
    }
    if (argc >= 2 && strcmp(argv[1], "--help") == 0) {
        agc_live_print_usage(argv[0]);
        return 0;
    }

    memset(&ctx, 0, sizeof(ctx));
    ctx.sample_rate_hz = 16000;
    ctx.input_device_id = WAVE_MAPPER;
    ctx.output_device_id = WAVE_MAPPER;
    ctx.meter_enabled = 1;

    if (argc >= 2) {
        ctx.sample_rate_hz = atoi(argv[1]);
    }
    if (argc >= 3) {
        target_rms_percent = (uint16_t)atoi(argv[2]);
    }
    if (argc >= 4) {
        max_gain_db = (uint16_t)atoi(argv[3]);
    }
    if (argc >= 5) {
        ctx.input_device_id = (UINT)atoi(argv[4]);
    }
    if (argc >= 6) {
        ctx.output_device_id = (UINT)atoi(argv[5]);
    }
    if (argc >= 7) {
        record_prefix = argv[6];
        ctx.record_enabled = 1;
    }

    agc_fixed_runtime_init(&ctx.runtime,
                           ctx.sample_rate_hz,
                           target_rms_percent,
                           max_gain_db);
    ctx.frame_samples = agc_fixed_runtime_frame_samples(&ctx.runtime);
    if (ctx.frame_samples <= 0 || ctx.frame_samples > AGC_FIXED_MAX_FRAME_SAMPLES) {
        fprintf(stderr, "Invalid frame size: %d\n", ctx.frame_samples);
        return 1;
    }

    if (ctx.record_enabled) {
        char input_path[MAX_PATH];
        char output_path[MAX_PATH];

        snprintf(input_path, sizeof(input_path), "%s_input.wav", record_prefix);
        snprintf(output_path, sizeof(output_path), "%s_output.wav", record_prefix);

        if (!agc_live_wav_open(&ctx.input_writer, input_path, ctx.sample_rate_hz) ||
            !agc_live_wav_open(&ctx.output_writer, output_path, ctx.sample_rate_hz)) {
            fprintf(stderr, "Failed to open record files.\n");
            agc_live_wav_close(&ctx.input_writer);
            agc_live_wav_close(&ctx.output_writer);
            return 1;
        }
    }

    if (!agc_live_open_devices(&ctx)) {
        fprintf(stderr, "Failed to open audio devices.\n");
        agc_live_close_devices(&ctx);
        agc_live_wav_close(&ctx.input_writer);
        agc_live_wav_close(&ctx.output_writer);
        return 1;
    }

    printf("AGC live loopback started.\n");
    printf("sample_rate=%ld frame_samples=%d target_rms=%u%% max_gain=%u dB\n",
           (long)ctx.sample_rate_hz,
           ctx.frame_samples,
           target_rms_percent,
           max_gain_db);
    printf("input_device=%u output_device=%u record=%s\n",
           (unsigned)ctx.input_device_id,
           (unsigned)ctx.output_device_id,
           ctx.record_enabled ? "on" : "off");
    printf("Use a headset to avoid acoustic feedback. Press any key to stop.\n");

    mmr = waveInStart(ctx.wave_in);
    if (mmr != MMSYSERR_NOERROR) {
        fprintf(stderr, "Failed to start capture.\n");
        agc_live_close_devices(&ctx);
        agc_live_wav_close(&ctx.input_writer);
        agc_live_wav_close(&ctx.output_writer);
        return 1;
    }

    while (running) {
        for (i = 0; i < AGC_LIVE_BUFFER_COUNT; ++i) {
            if ((ctx.input_headers[i].dwFlags & WHDR_DONE) == 0) {
                continue;
            }

            if (!agc_live_wait_for_output_slot(&ctx, i)) {
                running = 0;
                break;
            }

            agc_fixed_runtime_process(&ctx.runtime,
                                      ctx.input_buffers[i],
                                      ctx.output_buffers[i],
                                      0);

            ctx.processed_frames++;
            agc_live_print_meter(&ctx, ctx.input_buffers[i], ctx.output_buffers[i]);

            if (ctx.record_enabled) {
                agc_live_wav_append(&ctx.input_writer, ctx.input_buffers[i], ctx.frame_samples);
                agc_live_wav_append(&ctx.output_writer, ctx.output_buffers[i], ctx.frame_samples);
            }

            ctx.output_headers[i].dwBufferLength =
                (DWORD)(ctx.frame_samples * (int16_t)sizeof(ctx.output_buffers[i][0]));
            mmr = waveOutWrite(ctx.wave_out, &ctx.output_headers[i], sizeof(ctx.output_headers[i]));
            if (mmr != MMSYSERR_NOERROR) {
                fprintf(stderr, "\nwaveOutWrite failed.\n");
                running = 0;
                break;
            }

            ctx.input_headers[i].dwFlags &= (DWORD)~WHDR_DONE;
            mmr = waveInAddBuffer(ctx.wave_in, &ctx.input_headers[i], sizeof(ctx.input_headers[i]));
            if (mmr != MMSYSERR_NOERROR) {
                fprintf(stderr, "\nwaveInAddBuffer failed.\n");
                running = 0;
                break;
            }
        }

        if (_kbhit()) {
            (void)_getch();
            running = 0;
        } else {
            Sleep(1);
        }
    }

    printf("\n");
    agc_live_close_devices(&ctx);
    agc_live_wav_close(&ctx.input_writer);
    agc_live_wav_close(&ctx.output_writer);
    printf("AGC live loopback stopped.\n");
    return 0;
}
