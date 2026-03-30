#include "agc_test_vectors.h"

#include <math.h>
#include <stdlib.h>

#ifndef AGC_PI
#define AGC_PI 3.14159265358979323846f
#endif

TestVector agc_make_step_vector(size_t sample_count, size_t step_index) {
    TestVector vector;
    size_t i;

    vector.name = "step";
    vector.sample_count = sample_count;
    vector.samples = (float *)calloc(sample_count, sizeof(float));

    if (vector.samples == NULL) {
        vector.sample_count = 0U;
        return vector;
    }

    for (i = step_index; i < sample_count; ++i) {
        vector.samples[i] = 0.25f;
    }

    return vector;
}

TestVector agc_make_tone_burst_vector(size_t sample_count,
                                      int sample_rate_hz,
                                      float tone_hz,
                                      float amplitude,
                                      size_t start_index,
                                      size_t end_index) {
    TestVector vector;
    size_t i;

    vector.name = "tone_burst";
    vector.sample_count = sample_count;
    vector.samples = (float *)calloc(sample_count, sizeof(float));

    if (vector.samples == NULL) {
        vector.sample_count = 0U;
        return vector;
    }

    if (end_index > sample_count) {
        end_index = sample_count;
    }

    for (i = start_index; i < end_index; ++i) {
        float t = (float)i / (float)sample_rate_hz;
        vector.samples[i] = amplitude * sinf(2.0f * AGC_PI * tone_hz * t);
    }

    return vector;
}

TestVector agc_make_dual_tone_vector(size_t sample_count,
                                     int sample_rate_hz,
                                     float first_tone_hz,
                                     float second_tone_hz,
                                     float amplitude,
                                     size_t split_index) {
    TestVector vector;
    size_t i;

    vector.name = "dual_tone";
    vector.sample_count = sample_count;
    vector.samples = (float *)calloc(sample_count, sizeof(float));

    if (vector.samples == NULL) {
        vector.sample_count = 0U;
        return vector;
    }

    if (split_index > sample_count) {
        split_index = sample_count;
    }

    for (i = 0; i < sample_count; ++i) {
        float t = (float)i / (float)sample_rate_hz;
        float tone_hz = (i < split_index) ? first_tone_hz : second_tone_hz;
        vector.samples[i] = amplitude * sinf(2.0f * AGC_PI * tone_hz * t);
    }

    return vector;
}

TestVector agc_make_amplitude_step_tone_vector(size_t sample_count,
                                               int sample_rate_hz,
                                               float tone_hz,
                                               float low_amplitude,
                                               float high_amplitude,
                                               size_t step_index) {
    TestVector vector;
    size_t i;

    vector.name = "amplitude_step_tone";
    vector.sample_count = sample_count;
    vector.samples = (float *)calloc(sample_count, sizeof(float));

    if (vector.samples == NULL) {
        vector.sample_count = 0U;
        return vector;
    }

    if (step_index > sample_count) {
        step_index = sample_count;
    }

    for (i = 0; i < sample_count; ++i) {
        float t = (float)i / (float)sample_rate_hz;
        float amplitude = (i < step_index) ? low_amplitude : high_amplitude;
        vector.samples[i] = amplitude * sinf(2.0f * AGC_PI * tone_hz * t);
    }

    return vector;
}

TestVector agc_make_noisy_tone_vector(size_t sample_count,
                                      int sample_rate_hz,
                                      float tone_hz,
                                      float tone_amplitude,
                                      float noise_amplitude,
                                      unsigned int seed) {
    TestVector vector;
    size_t i;
    unsigned int rng = seed;

    vector.name = "noisy_tone";
    vector.sample_count = sample_count;
    vector.samples = (float *)calloc(sample_count, sizeof(float));

    if (vector.samples == NULL) {
        vector.sample_count = 0U;
        return vector;
    }

    for (i = 0; i < sample_count; ++i) {
        float t = (float)i / (float)sample_rate_hz;
        float tone = tone_amplitude * sinf(2.0f * AGC_PI * tone_hz * t);
        float noise;

        rng = 1664525U * rng + 1013904223U;
        noise = ((float)((rng >> 8) & 0xFFFFU) / 32768.0f) - 1.0f;
        vector.samples[i] = tone + noise_amplitude * noise;
    }

    return vector;
}

TestVector agc_make_speech_turns_vector(size_t sample_count,
                                        int sample_rate_hz,
                                        float tone_hz) {
    TestVector vector;
    size_t i;

    vector.name = "speech_turns";
    vector.sample_count = sample_count;
    vector.samples = (float *)calloc(sample_count, sizeof(float));

    if (vector.samples == NULL) {
        vector.sample_count = 0U;
        return vector;
    }

    for (i = 0; i < sample_count; ++i) {
        float t = (float)i / (float)sample_rate_hz;
        float envelope = 0.0f;

        if (t >= 0.15f && t < 0.45f) {
            envelope = 0.10f;
        } else if (t >= 0.60f && t < 0.95f) {
            envelope = 0.22f;
        } else if (t >= 1.10f && t < 1.35f) {
            envelope = 0.07f;
        } else if (t >= 1.55f && t < 1.90f) {
            envelope = 0.28f;
        }

        if (envelope > 0.0f) {
            float syllable = 0.35f + 0.65f * fabsf(sinf(2.0f * AGC_PI * 3.1f * t));
            vector.samples[i] = envelope * syllable * sinf(2.0f * AGC_PI * tone_hz * t);
        }
    }

    return vector;
}

TestVector agc_make_noisy_speech_turns_vector(size_t sample_count,
                                              int sample_rate_hz,
                                              float tone_hz,
                                              float noise_amplitude,
                                              unsigned int seed) {
    TestVector vector;
    size_t i;
    unsigned int rng = seed;

    vector.name = "noisy_speech_turns";
    vector.sample_count = sample_count;
    vector.samples = (float *)calloc(sample_count, sizeof(float));

    if (vector.samples == NULL) {
        vector.sample_count = 0U;
        return vector;
    }

    for (i = 0; i < sample_count; ++i) {
        float t = (float)i / (float)sample_rate_hz;
        float envelope = 0.0f;
        float speech = 0.0f;
        float noise;

        if (t >= 0.15f && t < 0.45f) {
            envelope = 0.10f;
        } else if (t >= 0.60f && t < 0.95f) {
            envelope = 0.22f;
        } else if (t >= 1.10f && t < 1.35f) {
            envelope = 0.07f;
        } else if (t >= 1.55f && t < 1.90f) {
            envelope = 0.28f;
        }

        if (envelope > 0.0f) {
            float syllable = 0.35f + 0.65f * fabsf(sinf(2.0f * AGC_PI * 3.1f * t));
            speech = envelope * syllable * sinf(2.0f * AGC_PI * tone_hz * t);
        }

        rng = 1664525U * rng + 1013904223U;
        noise = ((float)((rng >> 8) & 0xFFFFU) / 32768.0f) - 1.0f;
        vector.samples[i] = speech + noise_amplitude * noise;
    }

    return vector;
}

TestVector agc_make_noise_floor_step_vector(size_t sample_count,
                                            int sample_rate_hz,
                                            float tone_hz,
                                            float tone_amplitude,
                                            float low_noise_amplitude,
                                            float high_noise_amplitude,
                                            size_t noise_step_index,
                                            unsigned int seed) {
    TestVector vector;
    size_t i;
    unsigned int rng = seed;

    vector.name = "noise_floor_step";
    vector.sample_count = sample_count;
    vector.samples = (float *)calloc(sample_count, sizeof(float));

    if (vector.samples == NULL) {
        vector.sample_count = 0U;
        return vector;
    }

    if (noise_step_index > sample_count) {
        noise_step_index = sample_count;
    }

    for (i = 0; i < sample_count; ++i) {
        float t = (float)i / (float)sample_rate_hz;
        float noise_amplitude = (i < noise_step_index) ? low_noise_amplitude : high_noise_amplitude;
        float tone = tone_amplitude * sinf(2.0f * AGC_PI * tone_hz * t);
        float noise;

        rng = 1664525U * rng + 1013904223U;
        noise = ((float)((rng >> 8) & 0xFFFFU) / 32768.0f) - 1.0f;
        vector.samples[i] = tone + noise_amplitude * noise;
    }

    return vector;
}

TestVector agc_make_pause_recovery_vector(size_t sample_count,
                                          int sample_rate_hz,
                                          float tone_hz) {
    TestVector vector;
    size_t i;

    vector.name = "pause_recovery";
    vector.sample_count = sample_count;
    vector.samples = (float *)calloc(sample_count, sizeof(float));

    if (vector.samples == NULL) {
        vector.sample_count = 0U;
        return vector;
    }

    for (i = 0; i < sample_count; ++i) {
        float t = (float)i / (float)sample_rate_hz;
        float envelope = 0.0f;

        if (t >= 0.10f && t < 0.55f) {
            envelope = 0.16f;
        } else if (t >= 1.10f && t < 1.55f) {
            envelope = 0.16f;
        }

        if (envelope > 0.0f) {
            float syllable = 0.40f + 0.60f * fabsf(sinf(2.0f * AGC_PI * 4.0f * t));
            vector.samples[i] = envelope * syllable * sinf(2.0f * AGC_PI * tone_hz * t);
        }
    }

    return vector;
}

void agc_free_test_vector(TestVector *vector) {
    if (vector == NULL) {
        return;
    }
    free(vector->samples);
    vector->samples = NULL;
    vector->sample_count = 0U;
    vector->name = NULL;
}
