#ifndef AGC_FIXED_LIMITER_H
#define AGC_FIXED_LIMITER_H

#include "agc_fixed_config.h"
#include "agc_fixed_types.h"

void agc_fixed_limiter_init(AgcFixedLimiterState *state);
void agc_fixed_apply_limiter(agc_accum40_t *frame_q15,
                             agc_frame_count_t frame_samples,
                             AgcFixedLimiterState *state,
                             const AgcFixedConfig *config);

#endif
