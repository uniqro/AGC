#ifndef AGC_FIXED_PIPELINE_H
#define AGC_FIXED_PIPELINE_H

#include "agc_fixed_build.h"
#include "agc_fixed_config.h"
#include "agc_fixed_detector.h"
#include "agc_fixed_types.h"

typedef struct AgcFixedFrameMetrics {
    agc_q15_t input_peak_q15;
    agc_q15_t filtered_envelope_q15;
    agc_q15_t filtered_rms_q15;
    int16_t gate_open;
    agc_gain_q12_20_t desired_gain_q20;
    agc_gain_q12_20_t applied_gain_q20;
    int16_t peak_protector_active;
    int16_t limiter_active;
    agc_q15_t output_peak_q15;
    int16_t overflow_flag;
} AgcFixedFrameMetrics;

void agc_fixed_pipeline_init(AgcFixedPipelineState *state);
void agc_fixed_process_frame(const int16_t *input,
                             int16_t *output,
                             agc_frame_count_t frame_samples,
                             AgcFixedPipelineState *state,
                             const AgcFixedConfig *config,
                             AgcFixedFrameMetrics *metrics);

#endif
