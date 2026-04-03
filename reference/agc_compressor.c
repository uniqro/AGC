#include "agc_compressor.h"

#include <math.h>
#include <string.h>

static float agc_db_to_linear(float db) {
    return powf(10.0f, db / 20.0f);
}

static float agc_linear_to_db(float linear) {
    if (linear <= 1.0e-9f) {
        return -180.0f;
    }
    return 20.0f * log10f(linear);
}

static float agc_soft_knee_output_db(float input_db,
                                     float threshold_db,
                                     float ratio,
                                     float knee_db) {
    float half_knee = 0.5f * knee_db;
    float lower = threshold_db - half_knee;
    float upper = threshold_db + half_knee;

    if (ratio <= 1.0f) {
        return input_db;
    }
    if (knee_db <= 0.0f) {
        if (input_db <= threshold_db) {
            return input_db;
        }
        return threshold_db + (input_db - threshold_db) / ratio;
    }
    if (input_db <= lower) {
        return input_db;
    }
    if (input_db >= upper) {
        return threshold_db + (input_db - threshold_db) / ratio;
    }

    {
        float compressed_db = threshold_db + (input_db - threshold_db) / ratio;
        float t = (input_db - lower) / knee_db;
        return input_db + t * t * (compressed_db - input_db);
    }
}

void agc_compressor_init(CompressorState *state) {
    memset(state, 0, sizeof(*state));
}

void agc_apply_compressor(float *frame,
                          size_t frame_samples,
                          CompressorState *state,
                          const AgcConfig *config) {
    size_t i;

    state->is_active = 0;
    state->max_gain_reduction_db = 0.0f;

    if (!config->compressor_enabled || config->compressor_ratio <= 1.0f) {
        return;
    }

    for (i = 0; i < frame_samples; ++i) {
        float abs_value = fabsf(frame[i]);

        if (abs_value > 1.0e-9f) {
            float input_db = agc_linear_to_db(abs_value);
            float output_db = agc_soft_knee_output_db(input_db,
                                                      config->compressor_threshold_dbfs,
                                                      config->compressor_ratio,
                                                      config->compressor_knee_db);
            float gain_reduction_db = input_db - output_db;

            if (gain_reduction_db > 0.0f) {
                float gain = agc_db_to_linear(-gain_reduction_db);
                frame[i] *= gain;
                state->is_active = 1;
                if (gain_reduction_db > state->max_gain_reduction_db) {
                    state->max_gain_reduction_db = gain_reduction_db;
                }
            }
        }
    }
}
