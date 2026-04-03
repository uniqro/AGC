#ifndef AGC_CONFIG_H
#define AGC_CONFIG_H

#include <stdint.h>

typedef enum AgcMode {
    AGC_MODE_AM = 0,
    AGC_MODE_DIGITAL = 1
} AgcMode;

typedef struct AgcConfig {
    AgcMode mode;
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
    float gain_headroom_margin;
    int peak_headroom_cap_enabled;
    int compressor_enabled;
    float compressor_threshold_dbfs;
    float compressor_ratio;
    float compressor_knee_db;
    int peak_protector_enabled;
    float peak_protector_ratio;
    float limiter_threshold;

    float hp_cutoff_hz;
    float lp_cutoff_ratio;

    int32_t hp_num_taps;
    int32_t lp_num_taps;
} AgcConfig;

AgcConfig agc_config_default(int32_t sample_rate_hz);
AgcConfig agc_config_preset(int32_t sample_rate_hz, AgcMode mode);
const char *agc_mode_name(AgcMode mode);

#endif
