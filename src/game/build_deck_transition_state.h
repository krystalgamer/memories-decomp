#ifndef MEMORIES_DECOMP_BUILD_DECK_TRANSITION_STATE_H
#define MEMORIES_DECOMP_BUILD_DECK_TRANSITION_STATE_H

#include "../types.h"
#include "card_constants.h"
#include "card_list_text_boxes.h"

#define BUILD_DECK_TRANSITION_STATE_OFFSET(member) \
    ((u32)&(((BuildDeckTransitionState *)0)->member))

/* Shared Build Deck workspace used by the list, count, input, and transition
 * callbacks. The two CardList records account for the formerly repeated
 * +4/+0x2D50 views; the three card-indexed byte tables line up exactly at
 * +0x5AC4, +0x5D97, and +0x606A. */
typedef struct {
    u16 *deck_cards;
    CardList lists[2];
    s32 chest_total;
    s32 deck_total;
    s32 viewport_target_x;
    s32 viewport_step_x;
    s32 transition_ticks;
    u8 pad_5AB0[0x14];
    u8 deck_card_quantities[CARD_ID_END];
    u8 chest_card_quantities[CARD_ID_END];
    u8 card_sort_rank[CARD_ID_END + 1];
    u16 state;
    u16 next_state;
    u8 pane_index;
    u8 pad_6343;
} BuildDeckTransitionState;

typedef char BuildDeckTransitionState_target_offset_must_be_0x5AA4[
    BUILD_DECK_TRANSITION_STATE_OFFSET(viewport_target_x) == 0x5AA4 ? 1 : -1
];
typedef char BuildDeckTransitionState_lists_offset_must_be_0x4[
    BUILD_DECK_TRANSITION_STATE_OFFSET(lists) == 0x4 ? 1 : -1
];
typedef char BuildDeckTransitionState_deck_list_offset_must_be_0x2D50[
    BUILD_DECK_TRANSITION_STATE_OFFSET(lists[1]) == 0x2D50 ? 1 : -1
];
typedef char BuildDeckTransitionState_chest_total_offset_must_be_0x5A9C[
    BUILD_DECK_TRANSITION_STATE_OFFSET(chest_total) == 0x5A9C ? 1 : -1
];
typedef char BuildDeckTransitionState_deck_quantity_offset_must_be_0x5AC4[
    BUILD_DECK_TRANSITION_STATE_OFFSET(deck_card_quantities) == 0x5AC4 ? 1 : -1
];
typedef char BuildDeckTransitionState_chest_quantity_offset_must_be_0x5D97[
    BUILD_DECK_TRANSITION_STATE_OFFSET(chest_card_quantities) == 0x5D97 ? 1 : -1
];
typedef char BuildDeckTransitionState_sort_rank_offset_must_be_0x606A[
    BUILD_DECK_TRANSITION_STATE_OFFSET(card_sort_rank) == 0x606A ? 1 : -1
];
typedef char BuildDeckTransitionState_state_offset_must_be_0x633E[
    BUILD_DECK_TRANSITION_STATE_OFFSET(state) == 0x633E ? 1 : -1
];
typedef char BuildDeckTransitionState_pane_index_offset_must_be_0x6342[
    BUILD_DECK_TRANSITION_STATE_OFFSET(pane_index) == 0x6342 ? 1 : -1
];
typedef char BuildDeckTransitionState_size_must_be_0x6344[
    sizeof(BuildDeckTransitionState) == 0x6344 ? 1 : -1
];

#undef BUILD_DECK_TRANSITION_STATE_OFFSET

/* Read by the Build Deck C units but stored only in assembly:
 * func_800323F8.s:37 writes $s6 and reloads it at :217. The pointer identifies
 * one complete workspace because that function advances it by 0x6344 at :216.
 * func_80031874.s reads it at :57, :136, :230 and :242. Both assembly units
 * are without a profile in matching_c.json. Every global access is a %gp_rel
 * lw/sw (func_80032C48.s:258, func_80033998.s:4,
 * func_80033BE8.s:17/:31/:43), so the plain declaration. Initial value not
 * read.
 *
 * func_80033BE8 still spells the two scroll-box loads as byte offsets:
 * replacing those two expressions with the equivalent fields changes GCC
 * 2.8.1 instruction selection. The shared layout checks keep those preserved
 * raw accesses tied to the same record. */
extern BuildDeckTransitionState *D_8009B2FC;

#endif
