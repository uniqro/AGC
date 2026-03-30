#include "agc_fixed_limiter.h"

#include <string.h>

void agc_fixed_limiter_init(AgcFixedLimiterState *state) {
    memset(state, 0, sizeof(*state));
}

void agc_fixed_apply_limiter(agc_accum40_t *frame_q15,
                             agc_frame_count_t frame_samples,
                             AgcFixedLimiterState *state,
                             const AgcFixedConfig *config) {
    agc_frame_count_t i;

    state->is_active = 0;
    state->last_gain_reduction_q15 = 0;

    for (i = 0; i < frame_samples; ++i) {
        agc_accum40_t x = frame_q15[i];
        agc_accum40_t abs_x = (x < 0) ? -x : x;

        if (abs_x > config->limiter_threshold_q15) {
            agc_accum40_t reduction = abs_x - config->limiter_threshold_q15;
            state->is_active = 1;
            if (reduction > state->last_gain_reduction_q15) {
                state->last_gain_reduction_q15 = (agc_q15_t)reduction;
            }

            if (x > config->limiter_threshold_q15) {
                frame_q15[i] = config->limiter_threshold_q15;
            } else if (x < -config->limiter_threshold_q15) {
                frame_q15[i] = (int16_t)(-config->limiter_threshold_q15);
            }
        }
    }
}
