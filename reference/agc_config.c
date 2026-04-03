#include "agc_config.h"

const char *agc_mode_name(AgcMode mode) {
    switch (mode) {
        case AGC_MODE_AM:
            return "AM";
        case AGC_MODE_DIGITAL:
            return "DIGITAL";
        default:
            return "UNKNOWN";
    }
}

AgcConfig agc_config_preset(int32_t sample_rate_hz, AgcMode mode) {
    AgcConfig config;

    config.mode = mode;
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
    config.gain_headroom_margin = 1.00f;
    config.peak_headroom_cap_enabled = 0;
    config.compressor_enabled = 1;
    config.compressor_threshold_dbfs = -2.0f;
    config.compressor_ratio = 6.0f;
    config.compressor_knee_db = 8.0f;
    config.peak_protector_enabled = 0;
    config.peak_protector_ratio = 1.00f;
    config.limiter_threshold = 0.995f;

    config.hp_cutoff_hz = 250.0f;
    config.lp_cutoff_ratio = 0.4125f;

    config.hp_num_taps = 49;
    config.lp_num_taps = 49;

    switch (mode) {
        case AGC_MODE_AM:
            config.attack_ms = 8.0f;
            config.release_ms = 300.0f;
            config.gate_hold_ms = 80.0f;
            config.compressor_enabled = 1;
            config.compressor_threshold_dbfs = -2.0f;
            config.compressor_ratio = 6.0f;
            config.compressor_knee_db = 8.0f;
            config.peak_headroom_cap_enabled = 0;
            config.peak_protector_enabled = 0;
            config.peak_protector_ratio = 1.00f;
            config.limiter_threshold = 0.995f;
            break;

        case AGC_MODE_DIGITAL:
            config.attack_ms = 10.0f;
            config.release_ms = 350.0f;
            config.gate_hold_ms = 80.0f;
            config.compressor_enabled = 1;
            config.compressor_threshold_dbfs = -4.0f;
            config.compressor_ratio = 3.0f;
            config.compressor_knee_db = 6.0f;
            config.peak_headroom_cap_enabled = 1;
            config.peak_protector_enabled = 0;
            config.peak_protector_ratio = 1.00f;
            config.limiter_threshold = 0.990f;
            break;

        default:
            break;
    }

    return config;
}

AgcConfig agc_config_default(int32_t sample_rate_hz) {
    return agc_config_preset(sample_rate_hz, AGC_MODE_AM);
}
