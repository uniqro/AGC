#ifndef AGC_FIXED_PEAK_PROTECTOR_H
#define AGC_FIXED_PEAK_PROTECTOR_H

#include "agc_fixed_config.h"
#include "agc_fixed_types.h"

void agc_fixed_peak_protector_init(AgcFixedPeakProtectorState *state);
void agc_fixed_apply_peak_protector(agc_accum40_t *frame_q15,
                                    agc_frame_count_t frame_samples,
                                    AgcFixedPeakProtectorState *state,
                                    const AgcFixedConfig *config);

#endif
