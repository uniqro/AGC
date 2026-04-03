#ifndef AGC_COMPRESSOR_H
#define AGC_COMPRESSOR_H

#include <stddef.h>

#include "agc_config.h"
#include "agc_types.h"

void agc_compressor_init(CompressorState *state);
void agc_apply_compressor(float *frame,
                          size_t frame_samples,
                          CompressorState *state,
                          const AgcConfig *config);

#endif
