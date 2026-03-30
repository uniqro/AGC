#include "agc_level_detector.h"

#include <math.h>
#include <string.h>

void agc_level_detector_init(DetectorState *state) {
    memset(state, 0, sizeof(*state));
}

LevelInfo agc_measure_level(const float *input,
                            const float *filtered,
                            size_t frame_samples,
                            DetectorState *state,
                            const AgcConfig *config) {
    LevelInfo info;
    size_t i;
    float input_peak = 0.0f;
    float input_energy = 0.0f;
    float filtered_peak = 0.0f;
    float filtered_energy = 0.0f;
    float envelope_alpha;
    float rms_alpha;
    float envelope;
    float frame_rms;

    envelope_alpha = (float)frame_samples /
                     ((config->envelope_window_ms * 0.001f * (float)config->sample_rate_hz) + (float)frame_samples);
    if (envelope_alpha < 0.001f) {
        envelope_alpha = 0.001f;
    }
    if (envelope_alpha > 1.0f) {
        envelope_alpha = 1.0f;
    }
    rms_alpha = (float)frame_samples /
                ((config->rms_window_ms * 0.001f * (float)config->sample_rate_hz) + (float)frame_samples);
    if (rms_alpha < 0.001f) {
        rms_alpha = 0.001f;
    }
    if (rms_alpha > 1.0f) {
        rms_alpha = 1.0f;
    }
    envelope = state->envelope;

    for (i = 0; i < frame_samples; ++i) {
        float in_abs = fabsf(input[i]);
        float flt_abs = fabsf(filtered[i]);
        if (in_abs > input_peak) {
            input_peak = in_abs;
        }
        if (flt_abs > filtered_peak) {
            filtered_peak = flt_abs;
        }
        input_energy += input[i] * input[i];
        filtered_energy += filtered[i] * filtered[i];
        envelope += envelope_alpha * (flt_abs - envelope);
    }

    if (frame_samples > 0U) {
        input_energy /= (float)frame_samples;
        filtered_energy /= (float)frame_samples;
    }

    frame_rms = sqrtf(filtered_energy);
    state->envelope = envelope;
    state->smoothed_rms += rms_alpha * (frame_rms - state->smoothed_rms);

    info.input_peak = input_peak;
    info.input_energy = input_energy;
    info.filtered_peak = filtered_peak;
    info.filtered_energy = filtered_energy;
    info.filtered_rms = state->smoothed_rms;
    info.filtered_envelope = envelope;

    return info;
}
