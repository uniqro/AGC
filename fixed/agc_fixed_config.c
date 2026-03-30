#include "agc_fixed_config.h"

#include "agc_fixed_math.h"

static agc_q15_t agc_time_constant_to_alpha_q15(int32_t frame_ms, int32_t time_ms) {
    /*
     * Integer-friendly first-order approximation:
     * alpha ~= dt / (tau + dt)
     *
     * Good enough for the first DSP-oriented baseline and avoids float usage.
     */
    int32_t num;
    int32_t den;

    if (time_ms <= 0) {
        return AGC_Q15_ONE - 1;
    }

    num = frame_ms;
    den = time_ms + frame_ms;
    return agc_ratio_to_q15(num, den);
}

static agc_q15_t agc_window_to_alpha_q15(int32_t frame_samples,
                                         int32_t sample_rate_hz,
                                         int32_t window_ms) {
    int32_t window_samples;
    agc_q15_t alpha_q15;

    window_samples = (int32_t)(((int64_t)sample_rate_hz * window_ms) / 1000);
    alpha_q15 = agc_ratio_to_q15(frame_samples, window_samples + frame_samples);

    if (alpha_q15 < 33) {
        alpha_q15 = 33;
    } else if (alpha_q15 > (AGC_Q15_ONE - 1)) {
        alpha_q15 = AGC_Q15_ONE - 1;
    }

    return alpha_q15;
}

AgcFixedConfig agc_fixed_config_default(int32_t sample_rate_hz) {
    AgcFixedConfig cfg;

    cfg.sample_rate_hz = sample_rate_hz;
    /* Fixed baseline: 4 ms frames for low latency and modest DSP cost. */
    cfg.frame_ms = 4;
    cfg.frame_samples = (int16_t)((sample_rate_hz * cfg.frame_ms) / 1000);

    /* Reference-aligned operating point: target RMS around 42% FS. */
    cfg.target_rms_q15 = agc_ratio_to_q15(42, 100);
    /* Conservative gate threshold to avoid boosting pure background too easily. */
    cfg.gate_threshold_q15 = agc_ratio_to_q15(5, 100);
    /* Final hard safety threshold. */
    cfg.limiter_threshold_q15 = agc_ratio_to_q15(98, 100);
    /* Peak protector acts slightly before the limiter. */
    cfg.peak_protector_ratio_q15 = agc_ratio_to_q15(97, 100);

    /* Default max gain is 4.0x, matching the current reference baseline. */
    cfg.max_gain_q20 = agc_ratio_to_gain_q20(4, 1);

    /* Fast attack, slow release. */
    cfg.attack_alpha_q15 = agc_time_constant_to_alpha_q15(cfg.frame_ms, 8);
    cfg.release_alpha_q15 = agc_time_constant_to_alpha_q15(cfg.frame_ms, 300);
    /* Fast envelope for gate, slightly slower RMS for gain law stability. */
    cfg.envelope_alpha_q15 = agc_window_to_alpha_q15(cfg.frame_samples, sample_rate_hz, 4);
    cfg.rms_alpha_q15 = agc_window_to_alpha_q15(cfg.frame_samples, sample_rate_hz, 8);

    /* Hold the gate open briefly after activity falls to avoid chattering. */
    cfg.gate_hold_frames = (int16_t)(80 / cfg.frame_ms);
    if (cfg.gate_hold_frames < 0) {
        cfg.gate_hold_frames = 0;
    }

    return cfg;
}
