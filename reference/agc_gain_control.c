#include "agc_gain_control.h"

#include <math.h>
#include <string.h>

static float agc_time_constant_to_alpha(float time_ms, const AgcConfig *config) {
    float dt = (float)config->frame_ms * 0.001f;
    float tau = time_ms * 0.001f;

    if (tau <= 1.0e-6f) {
        return 1.0f;
    }
    return 1.0f - expf(-dt / tau);
}

static float agc_smoothstep(float edge0, float edge1, float x) {
    float t;

    if (edge1 <= edge0) {
        return x >= edge1 ? 1.0f : 0.0f;
    }
    t = (x - edge0) / (edge1 - edge0);
    if (t < 0.0f) {
        t = 0.0f;
    } else if (t > 1.0f) {
        t = 1.0f;
    }
    return t * t * (3.0f - 2.0f * t);
}

static float agc_peak_bias_weight(float blend_weight) {
    float keep_rms;

    if (blend_weight <= 0.0f) {
        return 0.0f;
    }
    if (blend_weight >= 1.0f) {
        return 1.0f;
    }

    /* In headroom-limited frames we want to move much closer to gain_peak
     * than a plain convex blend would. */
    keep_rms = 1.0f - blend_weight;
    return 1.0f - keep_rms * keep_rms * keep_rms * keep_rms;
}

static float agc_effective_target_peak_fs(const AgcConfig *config,
                                          float smoothed_crest_factor_db) {
    float target_peak = agc_target_peak_fs(config);
    float low_crest_target_peak;
    float high_crest_mix;

    /* Low-crest steady signals should not use the same peak target as
     * high-crest speech-like material. Pull the peak target closer to the
     * RMS target when crest factor stays low. */
    low_crest_target_peak =
        config->target_rms_fs + (target_peak - config->target_rms_fs) * 0.25f;
    high_crest_mix = agc_smoothstep(config->cf_low_db, config->cf_high_db,
                                    smoothed_crest_factor_db);
    return low_crest_target_peak
         + high_crest_mix * (target_peak - low_crest_target_peak);
}

void agc_gain_init(GainState *state) {
    memset(state, 0, sizeof(*state));
    state->applied_gain = 1.0f;
    state->desired_gain = 1.0f;
}

float agc_compute_desired_gain(const LevelInfo *level_info,
                               int gate_open,
                               GainState *state,
                               const AgcConfig *config) {
    float level;
    float gain_rms;
    float gain_peak;
    float gain;
    float target_peak = agc_target_peak_fs(config);
    float effective_target_peak = target_peak;
    float crest_factor_db = 0.0f;
    float cf_alpha = 0.0f;
    float blend_weight = 0.0f;
    float effective_blend_weight = 0.0f;
    int crest_update_allowed = 0;

    state->headroom_limited = 0;
    state->crest_smoothing_active = 0;

    if (!gate_open) {
        state->cf_blend_weight = 0.0f;
        return 1.0f;
    }

    level = level_info->filtered_rms;
    if (level <= 1.0e-9f) {
        state->cf_blend_weight = 0.0f;
        return 1.0f;
    }

    gain_rms = config->target_rms_fs / level;
    if (level_info->input_peak > 1.0e-6f) {
        gain_peak = target_peak / level_info->input_peak;
        crest_factor_db = 20.0f * log10f(level_info->input_peak / level);
    } else {
        gain_peak = config->max_gain;
    }
    state->current_crest_factor_db = crest_factor_db;

    crest_update_allowed = gate_open && level > config->rms_activity_floor;
    state->crest_smoothing_active = crest_update_allowed;
    if (crest_update_allowed) {
        cf_alpha = crest_factor_db > state->smoothed_crest_factor_db
                 ? agc_time_constant_to_alpha(config->cf_rise_ms, config)
                 : agc_time_constant_to_alpha(config->cf_fall_ms, config);
        state->smoothed_crest_factor_db += cf_alpha * (crest_factor_db - state->smoothed_crest_factor_db);
    }

    if (config->cfagc_enabled) {
        blend_weight = agc_smoothstep(config->cf_low_db,
                                      config->cf_high_db,
                                      state->smoothed_crest_factor_db);
    }
    effective_target_peak = agc_effective_target_peak_fs(
        config, state->smoothed_crest_factor_db);
    if (level_info->input_peak > 1.0e-6f) {
        gain_peak = effective_target_peak / level_info->input_peak;
    }

    /* Keep RMS-dominant behavior in low/mid CF frames, but blend in log-gain
     * space so peak protection pulls harder before the limiter has to react. */
    effective_blend_weight = blend_weight * blend_weight;
    if (gain_peak < gain_rms && blend_weight > 0.0f) {
        effective_blend_weight = agc_peak_bias_weight(blend_weight);
    }

    if (gain_rms > 1.0e-9f && gain_peak > 1.0e-9f) {
        gain = expf((1.0f - effective_blend_weight) * logf(gain_rms)
                    + effective_blend_weight * logf(gain_peak));
    } else {
        gain = (1.0f - effective_blend_weight) * gain_rms
             + effective_blend_weight * gain_peak;
    }

    if (gain_peak < gain_rms && effective_blend_weight > 0.0f) {
        state->headroom_limited = 1;
    }
    state->cf_blend_weight = effective_blend_weight;
    if (gain > config->max_gain) {
        gain = config->max_gain;
    }
    if (gain < 0.0f) {
        gain = 0.0f;
    }
    return gain;
}

float agc_smooth_gain(float desired_gain,
                      int fast_rise,
                      GainState *state,
                      const AgcConfig *config) {
    float alpha;
    float current = state->applied_gain;

    /* Fast gain reduction on sudden loud input, with optional fast rise on speech onset. */
    if (desired_gain < current || (fast_rise && desired_gain > current)) {
        alpha = agc_time_constant_to_alpha(config->attack_ms, config);
    } else {
        alpha = agc_time_constant_to_alpha(config->release_ms, config);
    }

    state->desired_gain = desired_gain;
    state->applied_gain = current + alpha * (desired_gain - current);
    return state->applied_gain;
}

void agc_apply_gain(const float *input,
                    float *output,
                    size_t frame_samples,
                    float gain,
                    GainState *state) {
    size_t i;

    if (gain < 0.0f) {
        gain = 0.0f;
    }

    state->overflow_detected = 0;
    for (i = 0; i < frame_samples; ++i) {
        float value = input[i] * gain;
        if (fabsf(input[i] * gain) > 1.0f) {
            state->overflow_detected = 1;
        }
        output[i] = value;
    }

    state->applied_gain = gain;
}
