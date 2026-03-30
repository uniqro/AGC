#ifndef AGC_GATE_H
#define AGC_GATE_H

#include "agc_config.h"
#include "agc_types.h"

void agc_gate_init(GateState *state);
int agc_update_gate(const LevelInfo *level_info,
                    GateState *state,
                    const AgcConfig *config);

#endif
