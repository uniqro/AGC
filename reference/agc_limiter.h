#ifndef AGC_LIMITER_H
#define AGC_LIMITER_H

#include <stddef.h>

#include "agc_config.h"
#include "agc_types.h"

void agc_limiter_init(LimiterState *state);
void agc_apply_limiter(float *frame,
                       size_t frame_samples,
                       LimiterState *state,
                       const AgcConfig *config);

#endif
