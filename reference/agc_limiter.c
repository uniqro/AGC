#include "agc_limiter.h"

#include <math.h>
#include <string.h>

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
            float reduction = abs_value - config->limiter_threshold;
            state->is_active = 1;
            if (reduction > state->last_gain_reduction) {
                state->last_gain_reduction = reduction;
            }
            if (frame[i] > config->limiter_threshold) {
                frame[i] = config->limiter_threshold;
            } else if (frame[i] < -config->limiter_threshold) {
                frame[i] = -config->limiter_threshold;
            }
        }
    }
}
