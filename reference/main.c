#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "agc_config.h"
#include "agc_pipeline.h"
#include "agc_test_vectors.h"
#include "agc_wav.h"

typedef struct RunSummary {
    unsigned long long active_gate_frames;
    unsigned long long peak_protector_frames;
    unsigned long long limiter_frames;
    float max_output_peak;
    float max_applied_gain;
} RunSummary;

static void update_summary(RunSummary *summary, const FrameMetrics *metrics) {
    if (metrics->gate_open) {
        summary->active_gate_frames++;
    }
    if (metrics->peak_protector_active) {
        summary->peak_protector_frames++;
    }
    if (metrics->limiter_active) {
        summary->limiter_frames++;
    }
    if (metrics->output_peak > summary->max_output_peak) {
        summary->max_output_peak = metrics->output_peak;
    }
    if (metrics->applied_gain > summary->max_applied_gain) {
        summary->max_applied_gain = metrics->applied_gain;
    }
}

static int run_vector(const char *prefix,
                      const TestVector *vector,
                      const AgcConfig *config) {
    PipelineState state;
    FrameMetrics metrics;
    RunSummary summary;
    float *output_signal;
    size_t frame_samples;
    size_t frame_start;
    char input_path[260];
    char output_path[260];

    agc_pipeline_init(&state);
    frame_samples = (size_t)config->frame_samples;
    memset(&summary, 0, sizeof(summary));

    output_signal = (float *)calloc(vector->sample_count, sizeof(float));
    if (output_signal == NULL) {
        fprintf(stderr, "Failed to allocate output buffer.\n");
        return 0;
    }

    memset(&metrics, 0, sizeof(metrics));
    for (frame_start = 0; frame_start < vector->sample_count; frame_start += frame_samples) {
        size_t current_frame = frame_samples;
        if (frame_start + current_frame > vector->sample_count) {
            current_frame = vector->sample_count - frame_start;
        }
        agc_process_frame(vector->samples + frame_start,
                          output_signal + frame_start,
                          current_frame,
                          &state,
                          config,
                          &metrics);
        update_summary(&summary, &metrics);
    }

    snprintf(input_path, sizeof(input_path), "%s_input.wav", prefix);
    snprintf(output_path, sizeof(output_path), "%s_output.wav", prefix);

    if (!agc_write_wav_mono_s16(input_path,
                                vector->samples,
                                vector->sample_count,
                                config->sample_rate_hz)) {
        fprintf(stderr, "Failed to write input WAV.\n");
    }

    if (!agc_write_wav_mono_s16(output_path,
                                output_signal,
                                vector->sample_count,
                                config->sample_rate_hz)) {
        fprintf(stderr, "Failed to write output WAV.\n");
    }

    printf("[%s] frames=%llu gate_frames=%llu peak_protector_frames=%llu limiter_frames=%llu last_gain=%.4f max_gain=%.4f max_output_peak=%.4f\n",
           prefix,
           (unsigned long long)state.frame_index,
           summary.active_gate_frames,
           summary.peak_protector_frames,
           summary.limiter_frames,
           metrics.applied_gain,
           summary.max_applied_gain,
           summary.max_output_peak);

    free(output_signal);
    return 1;
}

int main(void) {
    AgcConfig config = agc_config_default(16000);
    size_t total_samples = (size_t)config.sample_rate_hz * 2U;
    TestVector burst;
    TestVector step;
    TestVector step_hot;
    TestVector noisy;
    TestVector speech_turns;
    TestVector noisy_speech_turns;
    TestVector noise_floor_step;
    TestVector pause_recovery;

    burst = agc_make_tone_burst_vector(total_samples,
                                       config.sample_rate_hz,
                                       440.0f,
                                       0.25f,
                                       (size_t)(0.5f * (float)config.sample_rate_hz),
                                       (size_t)(1.5f * (float)config.sample_rate_hz));
    step = agc_make_amplitude_step_tone_vector(total_samples,
                                               config.sample_rate_hz,
                                               440.0f,
                                               0.05f,
                                               0.35f,
                                               total_samples / 2U);
    step_hot = agc_make_amplitude_step_tone_vector(total_samples,
                                                   config.sample_rate_hz,
                                                   440.0f,
                                                   0.05f,
                                                   0.75f,
                                                   total_samples / 2U);
    noisy = agc_make_noisy_tone_vector(total_samples,
                                       config.sample_rate_hz,
                                       440.0f,
                                       0.15f,
                                       0.05f,
                                       12345U);
    speech_turns = agc_make_speech_turns_vector(total_samples,
                                                config.sample_rate_hz,
                                                440.0f);
    noisy_speech_turns = agc_make_noisy_speech_turns_vector(total_samples,
                                                            config.sample_rate_hz,
                                                            440.0f,
                                                            0.035f,
                                                            24680U);
    noise_floor_step = agc_make_noise_floor_step_vector(total_samples,
                                                        config.sample_rate_hz,
                                                        440.0f,
                                                        0.14f,
                                                        0.01f,
                                                        0.07f,
                                                        total_samples / 2U,
                                                        67890U);
    pause_recovery = agc_make_pause_recovery_vector(total_samples,
                                                    config.sample_rate_hz,
                                                    440.0f);

    if (burst.samples == NULL || step.samples == NULL || step_hot.samples == NULL ||
        noisy.samples == NULL || speech_turns.samples == NULL || noisy_speech_turns.samples == NULL ||
        noise_floor_step.samples == NULL || pause_recovery.samples == NULL) {
        fprintf(stderr, "Failed to allocate one or more test vectors.\n");
        agc_free_test_vector(&burst);
        agc_free_test_vector(&step);
        agc_free_test_vector(&step_hot);
        agc_free_test_vector(&noisy);
        agc_free_test_vector(&speech_turns);
        agc_free_test_vector(&noisy_speech_turns);
        agc_free_test_vector(&noise_floor_step);
        agc_free_test_vector(&pause_recovery);
        return 1;
    }

    run_vector("reference_burst", &burst, &config);
    run_vector("reference_step", &step, &config);
    run_vector("reference_step_hot", &step_hot, &config);
    run_vector("reference_noisy", &noisy, &config);
    run_vector("reference_speech_turns", &speech_turns, &config);
    run_vector("reference_noisy_speech_turns", &noisy_speech_turns, &config);
    run_vector("reference_noise_floor_step", &noise_floor_step, &config);
    run_vector("reference_pause_recovery", &pause_recovery, &config);

    agc_free_test_vector(&burst);
    agc_free_test_vector(&step);
    agc_free_test_vector(&step_hot);
    agc_free_test_vector(&noisy);
    agc_free_test_vector(&speech_turns);
    agc_free_test_vector(&noisy_speech_turns);
    agc_free_test_vector(&noise_floor_step);
    agc_free_test_vector(&pause_recovery);
    return 0;
}
