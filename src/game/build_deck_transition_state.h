#ifndef MEMORIES_DECOMP_BUILD_DECK_TRANSITION_STATE_H
#define MEMORIES_DECOMP_BUILD_DECK_TRANSITION_STATE_H

#include "../types.h"

#define BUILD_DECK_TRANSITION_STATE_OFFSET(member) \
    ((u32)&(((BuildDeckTransitionState *)0)->member))

/* Shared Build Deck state used by the pane input and transition callbacks. */
typedef struct {
    u8 pad_0000[0x5AA4];
    s32 viewport_target_x;
    s32 viewport_step_x;
    s32 transition_ticks;
    u8 pad_5AB0[0x633E - 0x5AB0];
    u16 state;
    u16 next_state;
    u8 pane_index;
    u8 pad_6343;
} BuildDeckTransitionState;

typedef char BuildDeckTransitionState_target_offset_must_be_0x5AA4[
    BUILD_DECK_TRANSITION_STATE_OFFSET(viewport_target_x) == 0x5AA4 ? 1 : -1
];
typedef char BuildDeckTransitionState_state_offset_must_be_0x633E[
    BUILD_DECK_TRANSITION_STATE_OFFSET(state) == 0x633E ? 1 : -1
];
typedef char BuildDeckTransitionState_pane_index_offset_must_be_0x6342[
    BUILD_DECK_TRANSITION_STATE_OFFSET(pane_index) == 0x6342 ? 1 : -1
];

#undef BUILD_DECK_TRANSITION_STATE_OFFSET

#endif
