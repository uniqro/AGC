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
    float gain;
    float headroom_capped_gain = config->max_gain;
    float safe_peak = config->limiter_threshold *
                      config->peak_protector_ratio *
                      config->gain_headroom_margin;

    state->headroom_limited = 0;

    if (!gate_open) {
        return 1.0f;
    }

    level = level_info->filtered_rms;
    if (level <= 1.0e-9f) {
        return 1.0f;
    }

    gain = config->target_rms_fs / level;
    if (config->peak_headroom_cap_enabled && level_info->input_peak > 1.0e-6f) {
        headroom_capped_gain = safe_peak / level_info->input_peak;
        if (headroom_capped_gain < gain) {
            gain = headroom_capped_gain;
            state->headroom_limited = 1;
        }
    }
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
