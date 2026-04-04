#include "agc_metrics.h"

#include <string.h>

FrameMetrics agc_collect_metrics(const LevelInfo *level_info,
                                 int gate_open,
                                 const GainState *gain_state,
                                 const CompressorState *compressor_state,
                                 const PeakProtectorState *peak_protector_state,
                                 const LimiterState *limiter_state,
                                 float output_peak,
                                 float output_energy) {
    FrameMetrics metrics;

    memset(&metrics, 0, sizeof(metrics));
    metrics.input_peak = level_info->input_peak;
    metrics.input_energy = level_info->input_energy;
    metrics.filtered_energy = level_info->filtered_energy;
    metrics.filtered_rms = level_info->filtered_rms;
    metrics.filtered_envelope = level_info->filtered_envelope;
    metrics.gate_open = gate_open;
    metrics.desired_gain = gain_state->desired_gain;
    metrics.applied_gain = gain_state->applied_gain;
    metrics.crest_factor_db = gain_state->current_crest_factor_db;
    metrics.smoothed_crest_factor_db = gain_state->smoothed_crest_factor_db;
    metrics.cf_blend_weight = gain_state->cf_blend_weight;
    metrics.crest_smoothing_active = gain_state->crest_smoothing_active;
    metrics.headroom_limited = gain_state->headroom_limited;
    metrics.compressor_active = compressor_state->is_active;
    metrics.compressor_gain_reduction_db = compressor_state->max_gain_reduction_db;
    metrics.peak_protector_active = peak_protector_state->is_active;
    metrics.peak_protector_scale = peak_protector_state->applied_scale;
    metrics.limiter_active = limiter_state->is_active;
    metrics.output_peak = output_peak;
    metrics.output_energy = output_energy;
    metrics.overflow_flag = gain_state->overflow_detected;
    metrics.saturation_flag = limiter_state->is_active;

    return metrics;
}
