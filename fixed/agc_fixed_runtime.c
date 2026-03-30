#include "agc_fixed_runtime.h"

#include <string.h>

#include "agc_fixed_math.h"

static agc_gain_q12_20_t agc_runtime_db_to_gain_q20(uint16_t db) {
    static const agc_gain_q12_20_t table_q20[] = {
        1048576, 1176522, 1320079, 1481153, 1661881, 1864661, 2092184,
        2347469, 2633904, 2955289, 3315888, 3720488, 4174456, 4683817,
        5255329, 5896576, 6616067, 7423350, 8329135, 9345443, 10485760,
        11765216, 13200790, 14811530, 16618810, 18646611, 20921842,
        23474693, 26339038, 29552887, 33158885
    };
    enum { AGC_DB_TABLE_MAX = 30 };

    if (db >= AGC_DB_TABLE_MAX) {
        return table_q20[(sizeof(table_q20) / sizeof(table_q20[0])) - 1];
    }
    return table_q20[db];
}

void agc_fixed_runtime_init(AgcFixedRuntime *runtime,
                            int32_t sample_rate_hz,
                            uint16_t target_rms_percent,
                            uint16_t max_gain_db) {
    memset(runtime, 0, sizeof(*runtime));
    runtime->config = agc_fixed_config_default(sample_rate_hz);
    runtime->config.target_rms_q15 = agc_ratio_to_q15((int32_t)target_rms_percent, 100);
    runtime->config.max_gain_q20 = agc_runtime_db_to_gain_q20(max_gain_db);
    agc_fixed_pipeline_init(&runtime->pipeline);
}

int16_t agc_fixed_runtime_frame_samples(const AgcFixedRuntime *runtime) {
    return runtime->config.frame_samples;
}

agc_frame_count_t agc_fixed_runtime_buffered_samples(const AgcFixedRuntime *runtime) {
    return runtime->input_window_fill;
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

int agc_fixed_runtime_process_buffered(AgcFixedRuntime *runtime,
                                       const int16_t *input,
                                       agc_frame_count_t input_samples,
                                       agc_frame_count_t *consumed_samples,
                                       int16_t *output_frame,
                                       int *frame_ready,
                                       AgcFixedFrameMetrics *metrics) {
    agc_frame_count_t frame_samples;
    agc_frame_count_t remaining_space;
    agc_frame_count_t copy_count;

    if (runtime == 0 || input == 0 || consumed_samples == 0 ||
        output_frame == 0 || frame_ready == 0) {
        return 0;
    }

    frame_samples = (agc_frame_count_t)runtime->config.frame_samples;
    if (frame_samples == 0 || frame_samples > AGC_FIXED_MAX_FRAME_SAMPLES) {
        return 0;
    }
    if (runtime->input_window_fill > frame_samples) {
        return 0;
    }

    remaining_space = (agc_frame_count_t)(frame_samples - runtime->input_window_fill);
    copy_count = input_samples;
    if (copy_count > remaining_space) {
        copy_count = remaining_space;
    }

    if (copy_count > 0) {
        memcpy(&runtime->input_window[runtime->input_window_fill],
               input,
               (size_t)copy_count * sizeof(runtime->input_window[0]));
        runtime->input_window_fill = (agc_frame_count_t)(runtime->input_window_fill + copy_count);
    }

    *consumed_samples = copy_count;
    *frame_ready = 0;

    if (runtime->input_window_fill != frame_samples) {
        return 1;
    }

    agc_fixed_process_frame(runtime->input_window,
                            output_frame,
                            frame_samples,
                            &runtime->pipeline,
                            &runtime->config,
                            metrics);
    runtime->input_window_fill = 0;
    *frame_ready = 1;
    return 1;
}

int agc_fixed_runtime_process_sample(AgcFixedRuntime *runtime,
                                     int16_t input_sample,
                                     int16_t *output_sample,
                                     AgcFixedFrameMetrics *metrics) {
    agc_frame_count_t consumed_samples;
    int frame_ready;

    if (runtime == 0 || output_sample == 0) {
        return 0;
    }

    if (runtime->output_window_count > 0) {
        *output_sample = runtime->output_window[runtime->output_window_index];
        runtime->output_window_index =
            (agc_frame_count_t)(runtime->output_window_index + 1);
        runtime->output_window_count =
            (agc_frame_count_t)(runtime->output_window_count - 1);
    } else {
        *output_sample = 0;
    }

    if (!agc_fixed_runtime_process_buffered(runtime,
                                            &input_sample,
                                            1,
                                            &consumed_samples,
                                            runtime->output_window,
                                            &frame_ready,
                                            metrics)) {
        return 0;
    }

    if (consumed_samples != 1) {
        return 0;
    }

    if (frame_ready) {
        runtime->output_window_index = 0;
        runtime->output_window_count = (agc_frame_count_t)runtime->config.frame_samples;
    }

    return 1;
}
