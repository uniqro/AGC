#ifndef AGC_FIXED_RUNTIME_H
#define AGC_FIXED_RUNTIME_H

#include <stdint.h>

#include "agc_fixed_build.h"
#include "agc_fixed_config.h"
#include "agc_fixed_pipeline.h"

/*
 * DSP-facing runtime wrapper.
 *
 * Purpose:
 * - keep frame size fixed after init
 * - expose a simple int16 in/out API
 * - make the call pattern closer to the eventual TMS320 integration
 */

typedef struct AgcFixedRuntime {
    AgcFixedConfig config;
    AgcFixedPipelineState pipeline;
} AgcFixedRuntime;

void agc_fixed_runtime_init(AgcFixedRuntime *runtime, int32_t sample_rate_hz);
int16_t agc_fixed_runtime_frame_samples(const AgcFixedRuntime *runtime);
int agc_fixed_runtime_process(AgcFixedRuntime *runtime,
                              const int16_t *input,
                              int16_t *output,
                              AgcFixedFrameMetrics *metrics);

#endif
