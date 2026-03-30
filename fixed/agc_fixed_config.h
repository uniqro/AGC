#ifndef AGC_FIXED_CONFIG_H
#define AGC_FIXED_CONFIG_H

#include <stdint.h>

#include "agc_fixed_types.h"

typedef struct AgcFixedConfig {
    int32_t sample_rate_hz;
    int16_t frame_ms;
    int16_t frame_samples;

    agc_q15_t target_rms_q15;
    agc_q15_t gate_threshold_q15;
    agc_q15_t limiter_threshold_q15;
    agc_q15_t peak_protector_ratio_q15;

    agc_gain_q12_20_t max_gain_q20;

    /* Precomputed Q15 coefficients for the DSP path. */
    agc_q15_t attack_alpha_q15;
    agc_q15_t release_alpha_q15;
    agc_q15_t envelope_alpha_q15;
    agc_q15_t rms_alpha_q15;

    int16_t gate_hold_frames;
} AgcFixedConfig;

AgcFixedConfig agc_fixed_config_default(int32_t sample_rate_hz);

#endif
