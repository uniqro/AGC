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
    /* Frozen config values used by this runtime instance. */
    AgcFixedConfig config;
    /* Stateful DSP pipeline that advances frame by frame. */
    AgcFixedPipelineState pipeline;
    /*
     * Optional input window used by the buffered API.
     *
     * This lets the caller push partial sample chunks until one full internal
     * frame is accumulated. The classic full-frame API does not use this
     * buffer directly.
     */
    int16_t input_window[AGC_FIXED_MAX_FRAME_SAMPLES];
    agc_frame_count_t input_window_fill;
    /*
     * Output window used by the sample-by-sample API.
     *
     * One processed frame is stored here, then emitted one sample at a time.
     * This introduces one frame of intentional buffering latency.
     */
    int16_t output_window[AGC_FIXED_MAX_FRAME_SAMPLES];
    agc_frame_count_t output_window_index;
    agc_frame_count_t output_window_count;
} AgcFixedRuntime;

/*
 * User-facing init helper.
 *
 * Inputs:
 * - sample_rate_hz:
 *   audio sample rate, for example 16000
 *
 * - target_rms_percent:
 *   desired long-term RMS target as percent of full scale, for example 42
 *
 * - max_gain_db:
 *   maximum AGC gain in dB, for example 12 means 12 dB
 *
 * Notes:
 * - the caller does not need to know Q15 or Q12.20 formats
 * - values are converted internally into the fixed-point config
 *
 * Example:
 *   AgcFixedRuntime runtime;
 *   int16_t frame_samples;
 *
 *   agc_fixed_runtime_init(&runtime, 16000, 42, 12);
 *   frame_samples = agc_fixed_runtime_frame_samples(&runtime);
 *   agc_fixed_runtime_process(&runtime, input_frame, output_frame, 0);
 *
 * Buffered example:
 *   agc_frame_count_t consumed;
 *   int frame_ready;
 *
 *   agc_fixed_runtime_process_buffered(&runtime,
 *                                      input_chunk,
 *                                      chunk_samples,
 *                                      &consumed,
 *                                      output_frame,
 *                                      &frame_ready,
 *                                      0);
 *
 * Sample-by-sample example:
 *   int16_t output_sample;
 *
 *   agc_fixed_runtime_process_sample(&runtime,
 *                                    input_sample,
 *                                    &output_sample,
 *                                    0);
 */
void agc_fixed_runtime_init(AgcFixedRuntime *runtime,
                            int32_t sample_rate_hz,
                            uint16_t target_rms_percent,
                            uint16_t max_gain_db);

/*
 * Returns the exact frame size expected by agc_fixed_runtime_process(...).
 * Caller should query this once after init and shape its I/O buffering
 * accordingly.
 */
int16_t agc_fixed_runtime_frame_samples(const AgcFixedRuntime *runtime);

/*
 * Returns how many samples are currently buffered internally by the buffered
 * process path.
 *
 * This is useful when the caller feeds ISR/DMA fragments that are smaller than
 * one AGC frame and wants to observe how close the runtime is to producing the
 * next output frame.
 */
agc_frame_count_t agc_fixed_runtime_buffered_samples(const AgcFixedRuntime *runtime);

/*
 * Process exactly one full frame.
 *
 * Requirements:
 * - input and output must point to valid buffers of frame_samples length
 * - caller should provide separate input/output buffers
 * - metrics may be NULL
 *
 * Return value:
 * - 1 on accepted processing
 * - 0 on invalid frame/runtime state
 */
int agc_fixed_runtime_process(AgcFixedRuntime *runtime,
                              const int16_t *input,
                              int16_t *output,
                              AgcFixedFrameMetrics *metrics);

/*
 * Buffered processing helper for partial input chunks.
 *
 * Purpose:
 * - accept input blocks smaller than one AGC frame
 * - internally accumulate samples until one full frame is ready
 * - process at most one output frame per call
 *
 * Inputs:
 * - input:
 *   pointer to the next input chunk
 * - input_samples:
 *   number of valid samples in that chunk
 *
 * Outputs:
 * - consumed_samples:
 *   how many input samples were copied into the internal frame window
 * - output_frame:
 *   receives one processed frame when frame_ready becomes 1
 * - frame_ready:
 *   set to 1 when one full frame was produced, otherwise 0
 *
 * Notes:
 * - caller may pass chunks smaller or larger than the remaining frame space
 * - one call produces at most one frame
 * - to drain a larger input block, caller can loop until all input is consumed
 * - output_frame must point to a full frame-sized buffer
 * - metrics may be NULL
 *
 * Example:
 *   agc_frame_count_t consumed;
 *   int frame_ready;
 *
 *   while (remaining > 0) {
 *       agc_fixed_runtime_process_buffered(&runtime,
 *                                          input_ptr,
 *                                          remaining,
 *                                          &consumed,
 *                                          output_frame,
 *                                          &frame_ready,
 *                                          0);
 *       input_ptr += consumed;
 *       remaining -= consumed;
 *       if (frame_ready) {
 *           // one full AGC frame is now available in output_frame
 *       }
 *   }
 */
int agc_fixed_runtime_process_buffered(AgcFixedRuntime *runtime,
                                       const int16_t *input,
                                       agc_frame_count_t input_samples,
                                       agc_frame_count_t *consumed_samples,
                                       int16_t *output_frame,
                                       int *frame_ready,
                                       AgcFixedFrameMetrics *metrics);

/*
 * Single-sample streaming helper.
 *
 * Purpose:
 * - accept one input sample per call
 * - return one output sample per call
 * - internally preserve the frame-based AGC core
 *
 * Behavior:
 * - input samples are accumulated until one full frame is ready
 * - that frame is processed internally
 * - processed samples are then emitted one by one through output_sample
 * - before the first frame completes, output_sample is zero
 *
 * This API intentionally adds roughly one frame of buffering latency in
 * exchange for a simple sample-in / sample-out integration style.
 */
int agc_fixed_runtime_process_sample(AgcFixedRuntime *runtime,
                                     int16_t input_sample,
                                     int16_t *output_sample,
                                     AgcFixedFrameMetrics *metrics);

#endif
