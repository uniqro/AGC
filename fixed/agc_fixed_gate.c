#include "agc_fixed_gate.h"

#include <string.h>

static agc_q15_t agc_gate_close_threshold_q15(agc_q15_t open_threshold_q15) {
    return (agc_q15_t)(((int32_t)open_threshold_q15 * 7) / 10);
}

void agc_fixed_gate_init(AgcFixedGateState *state) {
    memset(state, 0, sizeof(*state));
}

int16_t agc_fixed_update_gate(const AgcFixedLevelInfo *level_info,
                              AgcFixedGateState *state,
                              const AgcFixedConfig *config) {
    agc_q15_t activity_q15 = level_info->filtered_envelope_q15;
    agc_q15_t open_threshold_q15 = config->gate_threshold_q15;
    agc_q15_t close_threshold_q15 = agc_gate_close_threshold_q15(open_threshold_q15);

    if (!state->is_open && activity_q15 >= open_threshold_q15) {
        state->is_open = 1;
        state->hold_frames_remaining = config->gate_hold_frames;
    } else if (activity_q15 >= close_threshold_q15) {
        state->is_open = 1;
        state->hold_frames_remaining = config->gate_hold_frames;
    } else if (state->hold_frames_remaining > 0) {
        state->hold_frames_remaining--;
        state->is_open = 1;
    } else {
        state->is_open = 0;
    }

    return state->is_open;
}
