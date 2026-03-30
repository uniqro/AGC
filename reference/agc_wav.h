#ifndef AGC_WAV_H
#define AGC_WAV_H

#include <stddef.h>
#include <stdint.h>

int agc_write_wav_mono_s16(const char *path,
                           const float *samples,
                           size_t sample_count,
                           int sample_rate_hz);
int agc_write_wav_mono_s16_raw(const char *path,
                               const int16_t *samples,
                               size_t sample_count,
                               int sample_rate_hz);

#endif
