#include "agc_pipeline.h"

#include <math.h>
#include <string.h>

#include "agc_gate.h"
#include "agc_gain_control.h"
#include "agc_level_detector.h"
#include "agc_limiter.h"
#include "agc_metrics.h"
#include "agc_peak_protector.h"

void agc_pipeline_init(PipelineState *state) {
    memset(state, 0, sizeof(*state));
    agc_level_detector_init(&state->detector);
    agc_gate_init(&state->gate);
    agc_gain_init(&state->gain);
    agc_peak_protector_init(&state->peak_protector);
    agc_limiter_init(&state->limiter);
}

void agc_process_frame(const float *input,
                       float *output,
                       size_t frame_samples,
                       PipelineState *state,
                       const AgcConfig *config,
                       FrameMetrics *metrics) {
    size_t i;
    float gained[512];
    LevelInfo level_info;
    int previous_gate_open;
    int gate_open;
    int fast_rise;
    float desired_gain;
    float applied_gain;
    float output_peak = 0.0f;
    float output_energy = 0.0f;

    if (frame_samples > 512U) {
        /* TODO: Replace with caller-managed buffers if larger frames are needed. */
        frame_samples = 512U;
    }

    previous_gate_open = state->gate.is_open;
    level_info = agc_measure_level(input, input, frame_samples, &state->detector, config);
    gate_open = agc_update_gate(&level_info, &state->gate, config);
    desired_gain = agc_compute_desired_gain(&level_info, gate_open, config);
    fast_rise = (!previous_gate_open && gate_open && desired_gain > state->gain.applied_gain);
    applied_gain = agc_smooth_gain(desired_gain, fast_rise, &state->gain, config);
    agc_apply_gain(input, gained, frame_samples, applied_gain, &state->gain);
    agc_apply_peak_protector(gained, frame_samples, &state->peak_protector, config);
    agc_apply_limiter(gained, frame_samples, &state->limiter, config);

    for (i = 0; i < frame_samples; ++i) {
        float abs_value = fabsf(gained[i]);
        output[i] = gained[i];
        if (abs_value > output_peak) {
            output_peak = abs_value;
        }
        output_energy += gained[i] * gained[i];
    }

    if (frame_samples > 0U) {
        output_energy /= (float)frame_samples;
    }

    if (metrics != NULL) {
        *metrics = agc_collect_metrics(&level_info,
                                       gate_open,
                                       &state->gain,
                                       &state->peak_protector,
                                       &state->limiter,
                                       output_peak,
                                       output_energy);
    }

    state->frame_index++;
}
