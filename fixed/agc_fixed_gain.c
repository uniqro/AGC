#include "agc_fixed_gain.h"

#include <string.h>

#include "agc_fixed_math.h"

void agc_fixed_gain_init(AgcFixedGainState *state) {
    memset(state, 0, sizeof(*state));
    state->desired_gain_q20 = AGC_GAIN_Q20_ONE;
    state->applied_gain_q20 = AGC_GAIN_Q20_ONE;
}

agc_gain_q12_20_t agc_fixed_compute_desired_gain(const AgcFixedLevelInfo *level_info,
                                                 int16_t gate_open,
                                                 const AgcFixedConfig *config) {
    agc_gain_q12_20_t gain_q20;

    if (!gate_open) {
        return AGC_GAIN_Q20_ONE;
    }

    if (level_info->filtered_rms_q15 <= 0) {
        return AGC_GAIN_Q20_ONE;
    }

    gain_q20 = agc_q15_div_to_gain_q20(config->target_rms_q15,
                                       level_info->filtered_rms_q15);
    if (gain_q20 > config->max_gain_q20) {
        gain_q20 = config->max_gain_q20;
    }
    if (gain_q20 < 0) {
        gain_q20 = 0;
    }

    return gain_q20;
}

agc_gain_q12_20_t agc_fixed_smooth_gain(agc_gain_q12_20_t desired_gain_q20,
                                        int16_t fast_rise,
                                        AgcFixedGainState *state,
                                        const AgcFixedConfig *config) {
    agc_q15_t alpha_q15;
    agc_accum40_t error_q20;
    agc_accum40_t delta_q20;

    if (desired_gain_q20 < state->applied_gain_q20 ||
        (fast_rise && desired_gain_q20 > state->applied_gain_q20)) {
        alpha_q15 = config->attack_alpha_q15;
    } else {
        alpha_q15 = config->release_alpha_q15;
    }

    /*
     * 40-bit candidate:
     * gain smoothing combines a Q20-domain error with a Q15 alpha. The
     * current 64-bit desktop expression is safe, but this is one of the main
     * spots to map deliberately onto the C54x wide accumulator model.
     */
    error_q20 = desired_gain_q20 - state->applied_gain_q20;
    delta_q20 = (agc_accum40_t)((error_q20 * alpha_q15) >> AGC_Q15_SHIFT);

    state->desired_gain_q20 = desired_gain_q20;
    state->applied_gain_q20 = (agc_gain_q12_20_t)((agc_accum40_t)state->applied_gain_q20 + delta_q20);
    return state->applied_gain_q20;
}

void agc_fixed_apply_gain_wide(const int16_t *input,
                               agc_accum40_t *output_q15,
                               agc_frame_count_t frame_samples,
                               agc_gain_q12_20_t gain_q20,
                               AgcFixedGainState *state) {
    agc_frame_count_t i;

    if (gain_q20 < 0) {
        gain_q20 = 0;
    }

    state->overflow_detected = 0;
    for (i = 0; i < frame_samples; ++i) {
        /*
         * 40-bit candidate:
         * input-by-gain multiplication is already kept wide in the pipeline.
         * If the target path later needs stricter saturation tracing, this is
         * the other natural place for an accumulator-backed implementation.
         */
        agc_accum40_t y = agc_q15_mul_gain_q20((agc_q15_t)input[i], gain_q20);
        output_q15[i] = y;
        if (y > 32767 || y < -32768) {
            state->overflow_detected = 1;
        }
    }

    state->applied_gain_q20 = gain_q20;
}
