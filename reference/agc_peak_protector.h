#ifndef AGC_PEAK_PROTECTOR_H
#define AGC_PEAK_PROTECTOR_H

#include <stddef.h>

#include "agc_config.h"
#include "agc_types.h"

void agc_peak_protector_init(PeakProtectorState *state);
void agc_apply_peak_protector(float *frame,
                              size_t frame_samples,
                              PeakProtectorState *state,
                              const AgcConfig *config);

#endif
