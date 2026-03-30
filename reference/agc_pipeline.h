#ifndef AGC_PIPELINE_H
#define AGC_PIPELINE_H

#include <stddef.h>

#include "agc_config.h"
#include "agc_types.h"

void agc_pipeline_init(PipelineState *state);
void agc_process_frame(const float *input,
                       float *output,
                       size_t frame_samples,
                       PipelineState *state,
                       const AgcConfig *config,
                       FrameMetrics *metrics);

#endif
