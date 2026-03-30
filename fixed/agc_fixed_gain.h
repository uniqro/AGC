#ifndef AGC_FIXED_GAIN_H
#define AGC_FIXED_GAIN_H

#include "agc_fixed_config.h"
#include "agc_fixed_detector.h"
#include "agc_fixed_types.h"

void agc_fixed_gain_init(AgcFixedGainState *state);
agc_gain_q12_20_t agc_fixed_compute_desired_gain(const AgcFixedLevelInfo *level_info,
                                                 int16_t gate_open,
                                                 const AgcFixedConfig *config);
agc_gain_q12_20_t agc_fixed_smooth_gain(agc_gain_q12_20_t desired_gain_q20,
                                        int16_t fast_rise,
                                        AgcFixedGainState *state,
                                        const AgcFixedConfig *config);
void agc_fixed_apply_gain_wide(const int16_t *input,
                               agc_accum40_t *output_q15,
                               agc_frame_count_t frame_samples,
                               agc_gain_q12_20_t gain_q20,
                               AgcFixedGainState *state);

#endif
