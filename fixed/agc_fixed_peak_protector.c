#include "agc_fixed_peak_protector.h"

#include <string.h>

#include "agc_fixed_math.h"

static agc_accum40_t agc_abs_q40(agc_accum40_t x) {
    if (x < 0) {
        return -x;
    }
    return x;
}

void agc_fixed_peak_protector_init(AgcFixedPeakProtectorState *state) {
    memset(state, 0, sizeof(*state));
    state->applied_scale_q15 = AGC_Q15_ONE;
}

void agc_fixed_apply_peak_protector(agc_accum40_t *frame_q15,
                                    agc_frame_count_t frame_samples,
                                    AgcFixedPeakProtectorState *state,
                                    const AgcFixedConfig *config) {
    agc_frame_count_t i;
    agc_accum40_t peak_q15 = 0;
    agc_q15_t safe_peak_q15;

    state->is_active = 0;
    state->applied_scale_q15 = AGC_Q15_ONE;
    safe_peak_q15 = (agc_q15_t)agc_q15_mul_q15(config->limiter_threshold_q15,
                                               config->peak_protector_ratio_q15);

    for (i = 0; i < frame_samples; ++i) {
        agc_accum40_t abs_q15 = agc_abs_q40(frame_q15[i]);
        if (abs_q15 > peak_q15) {
            peak_q15 = abs_q15;
        }
    }

    if (peak_q15 > safe_peak_q15 && peak_q15 > 0) {
        agc_q15_t scale_q15 = agc_ratio_to_q15(safe_peak_q15, (int32_t)peak_q15);
        state->is_active = 1;
        state->applied_scale_q15 = scale_q15;

        for (i = 0; i < frame_samples; ++i) {
            frame_q15[i] = agc_accum40_mul_q15(frame_q15[i], scale_q15);
        }
    }
}
