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
    u16 enabled;
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

/* The two row sets func_80061008 builds, and the array func_80060B38 and
 * func_80060E70 index by the display object's field_6A.
 *
 * This declaration is deliberately NOT the only one of D_801A8000 in the
 * tree, and must not be made so. 0x801A8000 is a shared staging buffer that
 * unrelated subsystems reinterpret wholesale: the script interpreter treats
 * it as bytecode (D_8009B290 = D_801A8000 + offset, in script_run_tick.c and
 * three siblings), the file-transfer steps hand its raw address to a DMA
 * descriptor word (value_08/value_0C, in file_transfer_steps.c and three
 * siblings), and two overlays view it as u32[] and MainMenuState[]. Those
 * are not worse-informed views of one record -- they are different records
 * living at one address at different times, so each keeps its own
 * declaration. Only the card-list group sees CardListRowSet here. */
extern CardListRowSet D_801A8000[];

/* The fourth word preserves caller setup; this body does not consume it. */
void func_80060E70(u16 *entries, s32 side, s32 flag, s32 ignored);

#endif
