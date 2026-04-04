#ifndef AGC_TYPES_H
#define AGC_TYPES_H

#include <stddef.h>
#include <stdint.h>

#define AGC_MAX_FIR_TAPS 64

typedef struct GateState {
    int is_open;
    int32_t hold_frames_remaining;
} GateState;

typedef struct DetectorState {
    float envelope;
    float smoothed_rms;
} DetectorState;

typedef struct GainState {
    float current_crest_factor_db;
    float desired_gain;
    float applied_gain;
    float smoothed_crest_factor_db;
    float cf_blend_weight;
    int crest_smoothing_active;
    int headroom_limited;
    int overflow_detected;
} GainState;

typedef struct CompressorState {
    int is_active;
    float max_gain_reduction_db;
} CompressorState;

typedef struct LimiterState {
    int is_active;
    float last_gain_reduction;
} LimiterState;

typedef struct PeakProtectorState {
    int is_active;
    float applied_scale;
} PeakProtectorState;

typedef struct PipelineState {
    DetectorState detector;
    GateState gate;
    GainState gain;
    CompressorState compressor;
    PeakProtectorState peak_protector;
    LimiterState limiter;
    uint64_t frame_index;
} PipelineState;

typedef struct LevelInfo {
    float input_peak;
    float input_energy;
    float filtered_peak;
    float filtered_energy;
    float filtered_rms;
    float filtered_envelope;
} LevelInfo;

typedef struct FrameMetrics {
    float input_peak;
    float input_energy;
    float filtered_energy;
    float filtered_rms;
    float filtered_envelope;
    int gate_open;
    float desired_gain;
    float applied_gain;
    float crest_factor_db;
    float smoothed_crest_factor_db;
    float cf_blend_weight;
    int crest_smoothing_active;
    int headroom_limited;
    int compressor_active;
    float compressor_gain_reduction_db;
    int peak_protector_active;
    float peak_protector_scale;
    int limiter_active;
    float output_peak;
    float output_energy;
    int saturation_flag;
    int overflow_flag;
} FrameMetrics;

#endif
