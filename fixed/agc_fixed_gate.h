#ifndef AGC_FIXED_GATE_H
#define AGC_FIXED_GATE_H

#include "agc_fixed_config.h"
#include "agc_fixed_detector.h"
#include "agc_fixed_types.h"

void agc_fixed_gate_init(AgcFixedGateState *state);
int16_t agc_fixed_update_gate(const AgcFixedLevelInfo *level_info,
                              AgcFixedGateState *state,
                              const AgcFixedConfig *config);

#endif
