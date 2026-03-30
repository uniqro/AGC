#ifndef AGC_TEST_VECTORS_H
#define AGC_TEST_VECTORS_H

#include <stddef.h>

typedef struct TestVector {
    const char *name;
    float *samples;
    size_t sample_count;
} TestVector;

TestVector agc_make_step_vector(size_t sample_count, size_t step_index);
TestVector agc_make_tone_burst_vector(size_t sample_count,
                                      int sample_rate_hz,
                                      float tone_hz,
                                      float amplitude,
                                      size_t start_index,
                                      size_t end_index);
TestVector agc_make_dual_tone_vector(size_t sample_count,
                                     int sample_rate_hz,
                                     float first_tone_hz,
                                     float second_tone_hz,
                                     float amplitude,
                                     size_t split_index);
TestVector agc_make_amplitude_step_tone_vector(size_t sample_count,
                                               int sample_rate_hz,
                                               float tone_hz,
                                               float low_amplitude,
                                               float high_amplitude,
                                               size_t step_index);
TestVector agc_make_noisy_tone_vector(size_t sample_count,
                                      int sample_rate_hz,
                                      float tone_hz,
                                      float tone_amplitude,
                                      float noise_amplitude,
                                      unsigned int seed);
TestVector agc_make_speech_turns_vector(size_t sample_count,
                                        int sample_rate_hz,
                                        float tone_hz);
TestVector agc_make_noisy_speech_turns_vector(size_t sample_count,
                                              int sample_rate_hz,
                                              float tone_hz,
                                              float noise_amplitude,
                                              unsigned int seed);
TestVector agc_make_noise_floor_step_vector(size_t sample_count,
                                            int sample_rate_hz,
                                            float tone_hz,
                                            float tone_amplitude,
                                            float low_noise_amplitude,
                                            float high_noise_amplitude,
                                            size_t noise_step_index,
                                            unsigned int seed);
TestVector agc_make_pause_recovery_vector(size_t sample_count,
                                          int sample_rate_hz,
                                          float tone_hz);
void agc_free_test_vector(TestVector *vector);

#endif
