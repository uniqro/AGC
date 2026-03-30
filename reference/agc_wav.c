#include "agc_wav.h"

#include <stdint.h>
#include <stdio.h>

static int agc_write_wav_header(FILE *fp,
                                size_t sample_count,
                                int sample_rate_hz) {
    uint32_t data_bytes;
    uint32_t riff_size;
    uint16_t audio_format = 1;
    uint16_t num_channels = 1;
    uint16_t bits_per_sample = 16;
    uint32_t byte_rate;
    uint16_t block_align;

    data_bytes = (uint32_t)(sample_count * sizeof(int16_t));
    riff_size = 36U + data_bytes;
    byte_rate = (uint32_t)(sample_rate_hz * num_channels * (bits_per_sample / 8U));
    block_align = (uint16_t)(num_channels * (bits_per_sample / 8U));

    fwrite("RIFF", 1, 4, fp);
    fwrite(&riff_size, sizeof(riff_size), 1, fp);
    fwrite("WAVE", 1, 4, fp);

    fwrite("fmt ", 1, 4, fp);
    {
        uint32_t fmt_size = 16U;
        fwrite(&fmt_size, sizeof(fmt_size), 1, fp);
    }
    fwrite(&audio_format, sizeof(audio_format), 1, fp);
    fwrite(&num_channels, sizeof(num_channels), 1, fp);
    fwrite(&sample_rate_hz, sizeof(sample_rate_hz), 1, fp);
    fwrite(&byte_rate, sizeof(byte_rate), 1, fp);
    fwrite(&block_align, sizeof(block_align), 1, fp);
    fwrite(&bits_per_sample, sizeof(bits_per_sample), 1, fp);

    fwrite("data", 1, 4, fp);
    fwrite(&data_bytes, sizeof(data_bytes), 1, fp);
    return 1;
}

static int16_t agc_float_to_s16(float x) {
    if (x > 1.0f) {
        x = 1.0f;
    } else if (x < -1.0f) {
        x = -1.0f;
    }

    if (x >= 0.0f) {
        return (int16_t)(x * 32767.0f + 0.5f);
    }
    return (int16_t)(x * 32768.0f - 0.5f);
}

int agc_write_wav_mono_s16(const char *path,
                           const float *samples,
                           size_t sample_count,
                           int sample_rate_hz) {
    FILE *fp;
    size_t i;

    fp = fopen(path, "wb");
    if (fp == NULL) {
        return 0;
    }

    agc_write_wav_header(fp, sample_count, sample_rate_hz);

    for (i = 0; i < sample_count; ++i) {
        int16_t s = agc_float_to_s16(samples[i]);
        fwrite(&s, sizeof(s), 1, fp);
    }

    fclose(fp);
    return 1;
}

int agc_write_wav_mono_s16_raw(const char *path,
                               const int16_t *samples,
                               size_t sample_count,
                               int sample_rate_hz) {
    FILE *fp;
    size_t i;

    fp = fopen(path, "wb");
    if (fp == NULL) {
        return 0;
    }

    agc_write_wav_header(fp, sample_count, sample_rate_hz);

    for (i = 0; i < sample_count; ++i) {
        fwrite(&samples[i], sizeof(samples[i]), 1, fp);
    }

    fclose(fp);
    return 1;
}
