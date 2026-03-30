#include "agc_config.h"

AgcConfig agc_config_default(int32_t sample_rate_hz) {
    AgcConfig config;

    config.sample_rate_hz = sample_rate_hz;
    config.frame_ms = 4;
    config.frame_samples = (int32_t)((sample_rate_hz * 4) / 1000);

    config.target_rms_fs = 0.42f;
    config.attack_ms = 8.0f;
    config.release_ms = 300.0f;
    config.envelope_window_ms = 4.0f;
    config.rms_window_ms = 8.0f;
    config.gate_hold_ms = 80.0f;

    config.gate_threshold = 0.05f;
    config.max_gain = 4.0f;
    config.peak_protector_ratio = 0.97f;
    config.limiter_threshold = 0.98f;

    config.hp_cutoff_hz = 250.0f;
    config.lp_cutoff_ratio = 0.4125f;

    config.hp_num_taps = 49;
    config.lp_num_taps = 49;

    return config;
}
