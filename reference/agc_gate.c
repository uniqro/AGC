#include "agc_gate.h"

#include <string.h>

void agc_gate_init(GateState *state) {
    memset(state, 0, sizeof(*state));
}

int agc_update_gate(const LevelInfo *level_info,
                    GateState *state,
                    const AgcConfig *config) {
    int hold_frames;
    float open_threshold = config->gate_threshold;
    float close_threshold = config->gate_threshold * 0.7f;
    float activity = level_info->filtered_envelope;

    if (!state->is_open && activity >= open_threshold) {
        hold_frames = (int)(config->gate_hold_ms / (float)config->frame_ms);
        if (hold_frames < 0) {
            hold_frames = 0;
        }
        state->is_open = 1;
        state->hold_frames_remaining = hold_frames;
    } else {
        if (activity >= close_threshold) {
            hold_frames = (int)(config->gate_hold_ms / (float)config->frame_ms);
            if (hold_frames < 0) {
                hold_frames = 0;
            }
            state->is_open = 1;
            state->hold_frames_remaining = hold_frames;
        } else if (state->hold_frames_remaining > 0) {
            state->hold_frames_remaining--;
            state->is_open = 1;
        } else {
            state->is_open = 0;
        }
    }

    return state->is_open;
}
