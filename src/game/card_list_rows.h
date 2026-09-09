#ifndef MEMORIES_DECOMP_CARD_LIST_ROWS_H
#define MEMORIES_DECOMP_CARD_LIST_ROWS_H

#include "../types.h"
#include "duel_card_display_state.h"

#define CARD_LIST_ROW_SET_OFFSET(member) \
    ((u32)&(((CardListRowSet *)0)->member))

/* One of the 24-byte D_801A8000 records shared by the row builder and
 * renderer. func_80060B38 independently confirms the enabled halfword and
 * seven card entries below. */
typedef struct {
    DuelCardDisplayObject *object;
    s16 x;
    s16 y;
    s16 enabled;
    u16 card_entries[7];
} CardListRowSet;

typedef char CardListRowSet_size_must_be_0x18[
    sizeof(CardListRowSet) == 0x18 ? 1 : -1
];
typedef char CardListRowSet_enabled_offset_must_be_0x8[
    CARD_LIST_ROW_SET_OFFSET(enabled) == 0x8 ? 1 : -1
];
typedef char CardListRowSet_entries_offset_must_be_0xA[
    CARD_LIST_ROW_SET_OFFSET(card_entries) == 0xA ? 1 : -1
];

#undef CARD_LIST_ROW_SET_OFFSET

/* The fourth word preserves caller setup; this body does not consume it. */
void func_80060E70(u16 *entries, s32 side, s32 flag, s32 ignored);

#endif
