#ifndef AGC_LEVEL_DETECTOR_H
#define AGC_LEVEL_DETECTOR_H

#include <stddef.h>

#include "agc_config.h"
#include "agc_types.h"

void agc_level_detector_init(DetectorState *state);
LevelInfo agc_measure_level(const float *input,
                            const float *filtered,
                            size_t frame_samples,
                            DetectorState *state,
                            const AgcConfig *config);

#endif
