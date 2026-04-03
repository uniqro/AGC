#ifndef AGC_GAIN_CONTROL_H
#define AGC_GAIN_CONTROL_H

#include <stddef.h>

#include "agc_config.h"
#include "agc_types.h"

void agc_gain_init(GainState *state);
float agc_compute_desired_gain(const LevelInfo *level_info,
                               int gate_open,
                               GainState *state,
                               const AgcConfig *config);
float agc_smooth_gain(float desired_gain,
                      int fast_rise,
                      GainState *state,
                      const AgcConfig *config);
void agc_apply_gain(const float *input,
                    float *output,
                    size_t frame_samples,
                    float gain,
                    GainState *state);

#endif
