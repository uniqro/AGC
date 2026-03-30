#include "agc_fixed_runtime.h"

#include <string.h>

void agc_fixed_runtime_init(AgcFixedRuntime *runtime, int32_t sample_rate_hz) {
    memset(runtime, 0, sizeof(*runtime));
    runtime->config = agc_fixed_config_default(sample_rate_hz);
    agc_fixed_pipeline_init(&runtime->pipeline);
}

int16_t agc_fixed_runtime_frame_samples(const AgcFixedRuntime *runtime) {
    return runtime->config.frame_samples;
}

int agc_fixed_runtime_process(AgcFixedRuntime *runtime,
                              const int16_t *input,
                              int16_t *output,
                              AgcFixedFrameMetrics *metrics) {
    int16_t frame_samples;

    frame_samples = runtime->config.frame_samples;
    if (frame_samples <= 0 || frame_samples > AGC_FIXED_MAX_FRAME_SAMPLES) {
        return 0;
    }

    agc_fixed_process_frame(input,
                            output,
                            (agc_frame_count_t)frame_samples,
                            &runtime->pipeline,
                            &runtime->config,
                            metrics);
    return 1;
}
