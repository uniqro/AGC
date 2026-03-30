#include "agc_fixed_pipeline.h"

#include <string.h>

#include "agc_fixed_build.h"
#include "agc_fixed_gate.h"
#include "agc_fixed_gain.h"
#include "agc_fixed_limiter.h"
#include "agc_fixed_math.h"
#include "agc_fixed_peak_protector.h"

void agc_fixed_pipeline_init(AgcFixedPipelineState *state) {
    memset(state, 0, sizeof(*state));
    agc_fixed_detector_init(&state->detector);
    agc_fixed_gate_init(&state->gate);
    agc_fixed_gain_init(&state->gain);
    agc_fixed_peak_protector_init(&state->peak_protector);
    agc_fixed_limiter_init(&state->limiter);
}

void agc_fixed_process_frame(const int16_t *input,
                             int16_t *output,
                             agc_frame_count_t frame_samples,
                             AgcFixedPipelineState *state,
                             const AgcFixedConfig *config,
                             AgcFixedFrameMetrics *metrics) {
    AgcFixedLevelInfo level_info;
    int16_t previous_gate_open;
    int16_t gate_open;
    int16_t fast_rise;
    agc_gain_q12_20_t desired_gain_q20;
    agc_gain_q12_20_t applied_gain_q20;
    agc_q15_t output_peak_q15 = 0;
    agc_frame_count_t i;

    if (frame_samples > AGC_FIXED_MAX_FRAME_SAMPLES) {
        memset(output, 0, frame_samples * sizeof(*output));
#if AGC_FIXED_ENABLE_METRICS
        if (metrics != NULL) {
            memset(metrics, 0, sizeof(*metrics));
            metrics->overflow_flag = 1;
        }
#else
        (void)metrics;
#endif
        state->gain.overflow_detected = 1;
        state->frame_index++;
        return;
    }

    previous_gate_open = state->gate.is_open;

    level_info = agc_fixed_measure_level(input, frame_samples, &state->detector, config);
    gate_open = agc_fixed_update_gate(&level_info, &state->gate, config);
    desired_gain_q20 = agc_fixed_compute_desired_gain(&level_info, gate_open, config);
    fast_rise = (!previous_gate_open &&
                 gate_open &&
                 desired_gain_q20 > state->gain.applied_gain_q20);
    applied_gain_q20 = agc_fixed_smooth_gain(desired_gain_q20, fast_rise, &state->gain, config);

    agc_fixed_apply_gain_wide(input,
                              state->gain_buffer_q15,
                              frame_samples,
                              applied_gain_q20,
                              &state->gain);
    /*
     * C54x placement note:
     * gain_buffer_q15 is the main transient wide-buffer in the fixed path.
     * If memory placement becomes critical, this buffer is the first object to
     * move into a target-specific section or scratch region.
     */
    agc_fixed_apply_peak_protector(state->gain_buffer_q15,
                                   frame_samples,
                                   &state->peak_protector,
                                   config);
    agc_fixed_apply_limiter(state->gain_buffer_q15,
                            frame_samples,
                            &state->limiter,
                            config);

    for (i = 0; i < frame_samples; ++i) {
        agc_q15_t abs_q15;
        output[i] = agc_sat_s16(state->gain_buffer_q15[i]);
        if (output[i] == -32768) {
            abs_q15 = 32767;
        } else {
            abs_q15 = (output[i] < 0) ? (agc_q15_t)(-output[i]) : (agc_q15_t)output[i];
        }
        if (abs_q15 > output_peak_q15) {
            output_peak_q15 = abs_q15;
        }
    }

#if AGC_FIXED_ENABLE_METRICS
    if (metrics != NULL) {
        memset(metrics, 0, sizeof(*metrics));
        metrics->input_peak_q15 = level_info.input_peak_q15;
        metrics->filtered_envelope_q15 = level_info.filtered_envelope_q15;
        metrics->filtered_rms_q15 = level_info.filtered_rms_q15;
        metrics->gate_open = gate_open;
        metrics->desired_gain_q20 = desired_gain_q20;
        metrics->applied_gain_q20 = state->gain.applied_gain_q20;
        metrics->peak_protector_active = state->peak_protector.is_active;
        metrics->limiter_active = state->limiter.is_active;
        metrics->output_peak_q15 = output_peak_q15;
        metrics->overflow_flag = state->gain.overflow_detected;
    }
#else
    (void)level_info;
    (void)gate_open;
    (void)desired_gain_q20;
    (void)output_peak_q15;
    (void)metrics;
#endif

    state->frame_index++;
}
