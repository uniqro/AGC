#include "agc_peak_protector.h"

#include <math.h>
#include <string.h>

void agc_peak_protector_init(PeakProtectorState *state) {
    memset(state, 0, sizeof(*state));
    state->applied_scale = 1.0f;
}

void agc_apply_peak_protector(float *frame,
                              size_t frame_samples,
                              PeakProtectorState *state,
                              const AgcConfig *config) {
    size_t i;
    float peak = 0.0f;
    float safe_peak = config->limiter_threshold * config->peak_protector_ratio;

    state->is_active = 0;
    state->applied_scale = 1.0f;

    if (!config->peak_protector_enabled) {
        return;
    }

    for (i = 0; i < frame_samples; ++i) {
        float abs_value = fabsf(frame[i]);
        if (abs_value > peak) {
            peak = abs_value;
        }
    }

    if (peak > safe_peak && peak > 1.0e-6f) {
        float scale = safe_peak / peak;
        state->is_active = 1;
        state->applied_scale = scale;

        for (i = 0; i < frame_samples; ++i) {
            frame[i] *= scale;
        }
    }
}
