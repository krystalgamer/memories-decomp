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

/* Read by three C functions; no C function stores it. func_80032C48 reads
 * the byte at +0x606A + card_id (card_list_sort.c:188-189), func_80033998
 * walks 0x10-byte entries from +0x2D50 (func_80033998.c:8-10), and
 * func_80033BE8 reads the pointers at +0x2D38 and +0x5A84 through
 * `(u8 *)base` (func_800339D0.c), calls
 * `D_80090DF8[D_8009B2FC->state & 0x3F]` with it (:38) and returns
 * `D_8009B2FC->state` (:41). The only store in the binary is in
 * assembly: func_800323F8.s:37 `sw $s6`, reloaded at :217; func_80031874.s
 * reads it at :57, :136, :230 and :242. Both are without a profile in
 * matching_c.json. Typed as this struct because func_800339D0.c
 * reaches `->state` through it and the byte offsets the other two units use
 * lie inside pad_0000 and pad_5AB0; func_800323F8.s:216 adds 0x6344, the
 * struct's declared size, to $s6 (`addiu $s6, $s6, 0x6344`), and :38 stores
 * `sb $s1, 0x6343($s6)`. Every access is a %gp_rel lw/sw
 * (func_80032C48.s:258, func_80033998.s:4, func_80033BE8.s:17/:31/:43), so
 * the plain declaration. No prototype
 * takes it typed: D_80090DF8's parameter is `u8 *` and the cast is at the
 * call. Initial value not read. */
extern BuildDeckTransitionState *D_8009B2FC;

#endif
