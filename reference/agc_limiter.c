#include "agc_limiter.h"

#include <math.h>
#include <string.h>

static float agc_soft_limit_sample(float x, float threshold) {
    float abs_value = fabsf(x);
    float sign = x >= 0.0f ? 1.0f : -1.0f;
    float headroom;
    float overshoot;
    float compressed;

    if (abs_value <= threshold) {
        return x;
    }

    headroom = 1.0f - threshold;
    if (headroom <= 1.0e-6f) {
        return sign * threshold;
    }

    overshoot = (abs_value - threshold) / headroom;
    compressed = threshold + headroom * tanhf(overshoot);
    if (compressed > 1.0f) {
        compressed = 1.0f;
    }
    return sign * compressed;
}

void agc_limiter_init(LimiterState *state) {
    memset(state, 0, sizeof(*state));
}

void agc_apply_limiter(float *frame,
                       size_t frame_samples,
                       LimiterState *state,
                       const AgcConfig *config) {
    size_t i;
    state->is_active = 0;
    state->last_gain_reduction = 0.0f;

    for (i = 0; i < frame_samples; ++i) {
        float abs_value = fabsf(frame[i]);
        if (abs_value > config->limiter_threshold) {
            float limited = agc_soft_limit_sample(frame[i], config->limiter_threshold);
            float reduction = abs_value - fabsf(limited);
            state->is_active = 1;
            if (reduction > state->last_gain_reduction) {
                state->last_gain_reduction = reduction;
            }
            frame[i] = limited;
        }
    }
}
