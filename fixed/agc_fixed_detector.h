#ifndef AGC_FIXED_DETECTOR_H
#define AGC_FIXED_DETECTOR_H

#include "agc_fixed_config.h"
#include "agc_fixed_types.h"

typedef struct AgcFixedLevelInfo {
    agc_q15_t input_peak_q15;
    agc_q15_t filtered_peak_q15;
    agc_q15_t filtered_envelope_q15;
    agc_q15_t filtered_rms_q15;
    agc_accum40_t input_energy_q30;
    agc_accum40_t filtered_energy_q30;
} AgcFixedLevelInfo;

void agc_fixed_detector_init(AgcFixedDetectorState *state);
AgcFixedLevelInfo agc_fixed_measure_level(const int16_t *input,
                                          agc_frame_count_t frame_samples,
                                          AgcFixedDetectorState *state,
                                          const AgcFixedConfig *config);

#endif
