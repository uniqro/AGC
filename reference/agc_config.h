#ifndef AGC_CONFIG_H
#define AGC_CONFIG_H

#include <stdint.h>

typedef struct AgcConfig {
    int32_t sample_rate_hz;
    int32_t frame_ms;
    int32_t frame_samples;

    float target_rms_fs;
    float attack_ms;
    float release_ms;
    float envelope_window_ms;
    float rms_window_ms;
    float gate_hold_ms;

    float gate_threshold;
    float max_gain;
    float peak_protector_ratio;
    float limiter_threshold;

    float hp_cutoff_hz;
    float lp_cutoff_ratio;

    int32_t hp_num_taps;
    int32_t lp_num_taps;
} AgcConfig;

AgcConfig agc_config_default(int32_t sample_rate_hz);

#endif
