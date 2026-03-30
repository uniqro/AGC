#ifndef AGC_FIXED_CONFIG_H
#define AGC_FIXED_CONFIG_H

#include <stdint.h>

#include "agc_fixed_types.h"

/*
 * Fixed-point configuration used by the DSP-facing AGC runtime.
 *
 * This structure contains only internal fixed-point values. User-facing code
 * should normally not fill these fields one by one unless it is intentionally
 * bypassing the simpler runtime init wrapper.
 */
typedef struct AgcFixedConfig {
    /* Sample rate used to derive frame size and detector coefficients. */
    int32_t sample_rate_hz;
    /* Frame duration in milliseconds. Current baseline uses 4 ms. */
    int16_t frame_ms;
    /* Samples per frame, derived from sample_rate_hz and frame_ms. */
    int16_t frame_samples;

    /* Target RMS level in Q15 full-scale units. */
    agc_q15_t target_rms_q15;
    /* Gate open threshold in Q15. */
    agc_q15_t gate_threshold_q15;
    /* Final limiter threshold in Q15. */
    agc_q15_t limiter_threshold_q15;
    /* Ratio applied before limiter to trigger frame-level peak protection. */
    agc_q15_t peak_protector_ratio_q15;

    /* Maximum allowed AGC gain in Q12.20 linear format. */
    agc_gain_q12_20_t max_gain_q20;

    /* Precomputed first-order smoothing coefficients for the DSP path. */
    agc_q15_t attack_alpha_q15;
    agc_q15_t release_alpha_q15;
    agc_q15_t envelope_alpha_q15;
    agc_q15_t rms_alpha_q15;

    /* Gate hold time expressed as frame count. */
    int16_t gate_hold_frames;
} AgcFixedConfig;

/*
 * Build the project baseline config for a given sample rate.
 *
 * Caller note:
 * - this returns internal fixed-point values
 * - user-facing code will usually prefer agc_fixed_runtime_init(...)
 */
AgcFixedConfig agc_fixed_config_default(int32_t sample_rate_hz);

#endif
