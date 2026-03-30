#include "agc_fixed_detector.h"

#include <string.h>

#include "agc_fixed_math.h"

static agc_q15_t agc_int16_to_q15(int16_t x) {
    return (agc_q15_t)x;
}

static agc_q15_t agc_abs_q15(agc_q15_t x) {
    if (x == (agc_q15_t)-32768) {
        return 32767;
    }
    return (x < 0) ? (agc_q15_t)(-x) : x;
}

static agc_q15_t agc_q30_energy_to_q15_rms(agc_accum40_t energy_q30) {
    uint64_t x;
    uint64_t bit;
    uint64_t root;

    if (energy_q30 <= 0) {
        return 0;
    }

    x = (uint64_t)energy_q30;
    bit = 1ULL << 62;
    root = 0;

    while (bit > x) {
        bit >>= 2;
    }

    while (bit != 0) {
        if (x >= root + bit) {
            x -= root + bit;
            root = (root >> 1) + bit;
        } else {
            root >>= 1;
        }
        bit >>= 2;
    }

    return agc_sat_q15((int32_t)root);
}

void agc_fixed_detector_init(AgcFixedDetectorState *state) {
    memset(state, 0, sizeof(*state));
}

AgcFixedLevelInfo agc_fixed_measure_level(const int16_t *input,
                                          agc_frame_count_t frame_samples,
                                          AgcFixedDetectorState *state,
                                          const AgcFixedConfig *config) {
    AgcFixedLevelInfo info;
    agc_frame_count_t i;
    agc_q15_t envelope_q15;
    agc_accum40_t input_energy_accum = 0;
    agc_accum40_t filtered_energy_accum = 0;
    agc_q15_t input_peak_q15 = 0;
    agc_q15_t filtered_peak_q15 = 0;
    agc_q15_t frame_rms_q15;

    memset(&info, 0, sizeof(info));
    envelope_q15 = state->envelope_q15;

    for (i = 0; i < frame_samples; ++i) {
        agc_q15_t x_q15 = agc_int16_to_q15(input[i]);
        agc_q15_t abs_q15 = agc_abs_q15(x_q15);
        int32_t env_error_q15;

        if (abs_q15 > input_peak_q15) {
            input_peak_q15 = abs_q15;
        }
        if (abs_q15 > filtered_peak_q15) {
            filtered_peak_q15 = abs_q15;
        }

        /*
         * 40-bit candidate:
         * frame energy is the clearest place where a C54x accumulator-backed
         * path could replace the current 32-bit sum if longer frames or extra
         * headroom are needed later.
         */
        input_energy_accum += (agc_accum40_t)((int32_t)x_q15 * (int32_t)x_q15);
        filtered_energy_accum += (agc_accum40_t)((int32_t)x_q15 * (int32_t)x_q15);

        env_error_q15 = (int32_t)abs_q15 - (int32_t)envelope_q15;
        envelope_q15 = agc_sat_q15((int32_t)envelope_q15 +
                                   agc_q15_mul_q15(config->envelope_alpha_q15,
                                                   (agc_q15_t)env_error_q15));
    }

    if (frame_samples > 0U) {
        input_energy_accum /= (agc_accum40_t)frame_samples;
        filtered_energy_accum /= (agc_accum40_t)frame_samples;
    }

    frame_rms_q15 = agc_q30_energy_to_q15_rms(filtered_energy_accum);
    state->envelope_q15 = envelope_q15;
    state->smoothed_rms_q15 = agc_sat_q15((int32_t)state->smoothed_rms_q15 +
                                          agc_q15_mul_q15(config->rms_alpha_q15,
                                                          (agc_q15_t)((int32_t)frame_rms_q15 -
                                                                      (int32_t)state->smoothed_rms_q15)));

    info.input_peak_q15 = input_peak_q15;
    info.filtered_peak_q15 = filtered_peak_q15;
    info.filtered_envelope_q15 = state->envelope_q15;
    info.filtered_rms_q15 = state->smoothed_rms_q15;
    info.input_energy_q30 = input_energy_accum;
    info.filtered_energy_q30 = filtered_energy_accum;

    return info;
}
