#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "agc_config.h"
#include "agc_fixed_build.h"
#include "agc_fixed_config.h"
#include "agc_fixed_math.h"
#include "agc_fixed_pipeline.h"
#include "agc_pipeline.h"
#include "agc_test_vectors.h"
#include "agc_wav.h"

#if !AGC_FIXED_ENABLE_METRICS
#error agc_fixed_compare requires AGC_FIXED_ENABLE_METRICS=1
#endif

typedef struct CompareSummary {
    unsigned long long reference_gate_frames;
    unsigned long long fixed_gate_frames;
    unsigned long long reference_peak_protector_frames;
    unsigned long long fixed_peak_protector_frames;
    unsigned long long reference_limiter_frames;
    unsigned long long fixed_limiter_frames;
    float max_reference_peak;
    float max_fixed_peak;
    float mean_absolute_error;
    float max_absolute_error;
} CompareSummary;

static int16_t agc_float_to_s16(float x) {
    if (x > 1.0f) {
        x = 1.0f;
    } else if (x < -1.0f) {
        x = -1.0f;
    }

    if (x >= 0.0f) {
        return (int16_t)(x * 32767.0f + 0.5f);
    }
    return (int16_t)(x * 32768.0f - 0.5f);
}

static float agc_s16_to_float(int16_t x) {
    if (x >= 0) {
        return (float)x / 32767.0f;
    }
    return (float)x / 32768.0f;
}

static void agc_update_compare_summary(CompareSummary *summary,
                                       const FrameMetrics *reference_metrics,
                                       const AgcFixedFrameMetrics *fixed_metrics) {
    if (reference_metrics->gate_open) {
        summary->reference_gate_frames++;
    }
    if (fixed_metrics->gate_open) {
        summary->fixed_gate_frames++;
    }
    if (reference_metrics->peak_protector_active) {
        summary->reference_peak_protector_frames++;
    }
    if (fixed_metrics->peak_protector_active) {
        summary->fixed_peak_protector_frames++;
    }
    if (reference_metrics->limiter_active) {
        summary->reference_limiter_frames++;
    }
    if (fixed_metrics->limiter_active) {
        summary->fixed_limiter_frames++;
    }
    if (reference_metrics->output_peak > summary->max_reference_peak) {
        summary->max_reference_peak = reference_metrics->output_peak;
    }
    {
        float fixed_peak = agc_s16_to_float(fixed_metrics->output_peak_q15);
        if (fixed_peak > summary->max_fixed_peak) {
            summary->max_fixed_peak = fixed_peak;
        }
    }
}

static void agc_finish_error_summary(CompareSummary *summary,
                                     const float *reference_output,
                                     const float *fixed_output,
                                     size_t sample_count) {
    double sum_abs_error = 0.0;
    float max_abs_error = 0.0f;
    size_t i;

    for (i = 0; i < sample_count; ++i) {
        float abs_error = fabsf(reference_output[i] - fixed_output[i]);
        sum_abs_error += abs_error;
        if (abs_error > max_abs_error) {
            max_abs_error = abs_error;
        }
    }

    summary->mean_absolute_error = (sample_count > 0)
                                       ? (float)(sum_abs_error / (double)sample_count)
                                       : 0.0f;
    summary->max_absolute_error = max_abs_error;
}

static int run_compare_vector(const char *prefix,
                              const TestVector *vector,
                              const AgcConfig *reference_config,
                              const AgcFixedConfig *fixed_config) {
    PipelineState reference_state;
    AgcFixedPipelineState fixed_state;
    FrameMetrics reference_metrics;
    AgcFixedFrameMetrics fixed_metrics;
    CompareSummary summary;
    float *reference_output = NULL;
    float *fixed_output = NULL;
    int16_t *fixed_input = NULL;
    int16_t *fixed_output_s16 = NULL;
    size_t frame_samples;
    size_t frame_start;
    char reference_output_path[260];
    char fixed_input_path[260];
    char fixed_output_path[260];
    size_t i;

    if (reference_config->frame_samples != fixed_config->frame_samples) {
        fprintf(stderr, "[%s] frame size mismatch.\n", prefix);
        return 0;
    }

    reference_output = (float *)calloc(vector->sample_count, sizeof(float));
    fixed_output = (float *)calloc(vector->sample_count, sizeof(float));
    fixed_input = (int16_t *)calloc(vector->sample_count, sizeof(int16_t));
    fixed_output_s16 = (int16_t *)calloc(vector->sample_count, sizeof(int16_t));
    if (reference_output == NULL || fixed_output == NULL ||
        fixed_input == NULL || fixed_output_s16 == NULL) {
        fprintf(stderr, "[%s] buffer allocation failed.\n", prefix);
        free(reference_output);
        free(fixed_output);
        free(fixed_input);
        free(fixed_output_s16);
        return 0;
    }

    for (i = 0; i < vector->sample_count; ++i) {
        fixed_input[i] = agc_float_to_s16(vector->samples[i]);
    }

    memset(&summary, 0, sizeof(summary));
    memset(&reference_metrics, 0, sizeof(reference_metrics));
    memset(&fixed_metrics, 0, sizeof(fixed_metrics));
    agc_pipeline_init(&reference_state);
    agc_fixed_pipeline_init(&fixed_state);

    frame_samples = (size_t)reference_config->frame_samples;
    for (frame_start = 0; frame_start < vector->sample_count; frame_start += frame_samples) {
        size_t current_frame = frame_samples;
        if (frame_start + current_frame > vector->sample_count) {
            current_frame = vector->sample_count - frame_start;
        }

        agc_process_frame(vector->samples + frame_start,
                          reference_output + frame_start,
                          current_frame,
                          &reference_state,
                          reference_config,
                          &reference_metrics);

        agc_fixed_process_frame(fixed_input + frame_start,
                                fixed_output_s16 + frame_start,
                                current_frame,
                                &fixed_state,
                                fixed_config,
                                &fixed_metrics);

        agc_update_compare_summary(&summary, &reference_metrics, &fixed_metrics);
    }

    for (i = 0; i < vector->sample_count; ++i) {
        fixed_output[i] = agc_s16_to_float(fixed_output_s16[i]);
    }
    agc_finish_error_summary(&summary, reference_output, fixed_output, vector->sample_count);

    snprintf(reference_output_path, sizeof(reference_output_path),
             "%s_reference_output.wav", prefix);
    snprintf(fixed_input_path, sizeof(fixed_input_path),
             "%s_fixed_input.wav", prefix);
    snprintf(fixed_output_path, sizeof(fixed_output_path),
             "%s_fixed_output.wav", prefix);

    agc_write_wav_mono_s16(reference_output_path,
                           reference_output,
                           vector->sample_count,
                           reference_config->sample_rate_hz);
    agc_write_wav_mono_s16_raw(fixed_input_path,
                               fixed_input,
                               vector->sample_count,
                               fixed_config->sample_rate_hz);
    agc_write_wav_mono_s16_raw(fixed_output_path,
                               fixed_output_s16,
                               vector->sample_count,
                               fixed_config->sample_rate_hz);

    printf("[%s] mae=%.6f max_err=%.6f ref_gate=%llu fixed_gate=%llu ref_pp=%llu fixed_pp=%llu ref_lim=%llu fixed_lim=%llu ref_peak=%.4f fixed_peak=%.4f\n",
           prefix,
           summary.mean_absolute_error,
           summary.max_absolute_error,
           summary.reference_gate_frames,
           summary.fixed_gate_frames,
           summary.reference_peak_protector_frames,
           summary.fixed_peak_protector_frames,
           summary.reference_limiter_frames,
           summary.fixed_limiter_frames,
           summary.max_reference_peak,
           summary.max_fixed_peak);

    free(reference_output);
    free(fixed_output);
    free(fixed_input);
    free(fixed_output_s16);
    return 1;
}

int main(void) {
    AgcConfig reference_config = agc_config_default(16000);
    AgcFixedConfig fixed_config = agc_fixed_config_default(16000);
    size_t total_samples = (size_t)reference_config.sample_rate_hz * 2U;
    TestVector burst;
    TestVector step;
    TestVector step_hot;
    TestVector noisy;
    TestVector speech_turns;
    TestVector noisy_speech_turns;
    TestVector noise_floor_step;
    TestVector pause_recovery;
    int ok = 1;

    burst = agc_make_tone_burst_vector(total_samples,
                                       reference_config.sample_rate_hz,
                                       440.0f,
                                       0.25f,
                                       (size_t)(0.5f * (float)reference_config.sample_rate_hz),
                                       (size_t)(1.5f * (float)reference_config.sample_rate_hz));
    step = agc_make_amplitude_step_tone_vector(total_samples,
                                               reference_config.sample_rate_hz,
                                               440.0f,
                                               0.05f,
                                               0.35f,
                                               total_samples / 2U);
    step_hot = agc_make_amplitude_step_tone_vector(total_samples,
                                                   reference_config.sample_rate_hz,
                                                   440.0f,
                                                   0.05f,
                                                   0.75f,
                                                   total_samples / 2U);
    noisy = agc_make_noisy_tone_vector(total_samples,
                                       reference_config.sample_rate_hz,
                                       440.0f,
                                       0.15f,
                                       0.05f,
                                       12345U);
    speech_turns = agc_make_speech_turns_vector(total_samples,
                                                reference_config.sample_rate_hz,
                                                440.0f);
    noisy_speech_turns = agc_make_noisy_speech_turns_vector(total_samples,
                                                            reference_config.sample_rate_hz,
                                                            440.0f,
                                                            0.035f,
                                                            24680U);
    noise_floor_step = agc_make_noise_floor_step_vector(total_samples,
                                                        reference_config.sample_rate_hz,
                                                        440.0f,
                                                        0.14f,
                                                        0.01f,
                                                        0.07f,
                                                        total_samples / 2U,
                                                        67890U);
    pause_recovery = agc_make_pause_recovery_vector(total_samples,
                                                    reference_config.sample_rate_hz,
                                                    440.0f);

    if (burst.samples == NULL || step.samples == NULL || step_hot.samples == NULL ||
        noisy.samples == NULL || speech_turns.samples == NULL ||
        noisy_speech_turns.samples == NULL || noise_floor_step.samples == NULL ||
        pause_recovery.samples == NULL) {
        fprintf(stderr, "Failed to allocate compare test vectors.\n");
        ok = 0;
    }

    if (ok) {
        ok &= run_compare_vector("compare_burst", &burst, &reference_config, &fixed_config);
        ok &= run_compare_vector("compare_step", &step, &reference_config, &fixed_config);
        ok &= run_compare_vector("compare_step_hot", &step_hot, &reference_config, &fixed_config);
        ok &= run_compare_vector("compare_noisy", &noisy, &reference_config, &fixed_config);
        ok &= run_compare_vector("compare_speech_turns", &speech_turns, &reference_config, &fixed_config);
        ok &= run_compare_vector("compare_noisy_speech_turns",
                                 &noisy_speech_turns,
                                 &reference_config,
                                 &fixed_config);
        ok &= run_compare_vector("compare_noise_floor_step",
                                 &noise_floor_step,
                                 &reference_config,
                                 &fixed_config);
        ok &= run_compare_vector("compare_pause_recovery",
                                 &pause_recovery,
                                 &reference_config,
                                 &fixed_config);
    }

    agc_free_test_vector(&burst);
    agc_free_test_vector(&step);
    agc_free_test_vector(&step_hot);
    agc_free_test_vector(&noisy);
    agc_free_test_vector(&speech_turns);
    agc_free_test_vector(&noisy_speech_turns);
    agc_free_test_vector(&noise_floor_step);
    agc_free_test_vector(&pause_recovery);

    return ok ? 0 : 1;
}
