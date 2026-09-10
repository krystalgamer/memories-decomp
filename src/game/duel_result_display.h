#ifndef MEMORIES_DECOMP_DUEL_RESULT_DISPLAY_H
#define MEMORIES_DECOMP_DUEL_RESULT_DISPLAY_H

#include "../types.h"
#include "display_object.h"

#define DUEL_RESULT_DISPLAY_CHILD_COUNT 10
#define DUEL_RESULT_DISPLAY_OFFSET(member) \
    ((u32)&(((DuelResultDisplayState *)0)->member))

typedef struct {
    DisplayObject *root;
    DisplayObject *children[DUEL_RESULT_DISPLAY_CHILD_COUNT];
    /* Duel_CalcRankScore seeds both of these with
       DUEL_RANK_SCORE_INITIAL and then accumulates every
       Duel_CalcRankScoreChange result into them, indexed by side. */
    s32 side_scores[2];
    /* Three, not two: Duel_CalcRankScore writes 0x34, 0x35 and 0x36
       as one group -- 0x44, a variant selector, and 0x45. */
    u8 page_text_ids[3];
    u8 page_index;
    u8 rank_tier;
    u8 is_tec_rank;
    u8 starchip_prize;
    u8 pad_3B;
    s16 dropped_card_id;
    u8 pad_3E[2];
} DuelResultDisplayState;

typedef char DuelResultDisplayState_children_offset_must_be_0x4[
    DUEL_RESULT_DISPLAY_OFFSET(children) == 0x4 ? 1 : -1
];
typedef char DuelResultDisplayState_page_text_ids_offset_must_be_0x34[
    DUEL_RESULT_DISPLAY_OFFSET(page_text_ids) == 0x34 ? 1 : -1
];
typedef char DuelResultDisplayState_page_index_offset_must_be_0x37[
    DUEL_RESULT_DISPLAY_OFFSET(page_index) == 0x37 ? 1 : -1
];
typedef char DuelResultDisplayState_rank_tier_offset_must_be_0x38[
    DUEL_RESULT_DISPLAY_OFFSET(rank_tier) == 0x38 ? 1 : -1
];
typedef char DuelResultDisplayState_starchip_prize_offset_must_be_0x3A[
    DUEL_RESULT_DISPLAY_OFFSET(starchip_prize) == 0x3A ? 1 : -1
];
typedef char DuelResultDisplayState_dropped_card_id_offset_must_be_0x3C[
    DUEL_RESULT_DISPLAY_OFFSET(dropped_card_id) == 0x3C ? 1 : -1
];
typedef char DuelResultDisplayState_size_must_be_0x40[
    sizeof(DuelResultDisplayState) == 0x40 ? 1 : -1
];

#undef DUEL_RESULT_DISPLAY_OFFSET

/* The record itself, or rather the pointer the game reaches it through. It
 * belongs beside the layout above because that layout is the whole of what
 * this header knows about it.
 *
 * Both C users only read the pointer: Duel_ShowResultPage walks it as
 * `D_8009B1E8->root` and `D_8009B1E8->page_text_ids[page]`, and
 * Duel_CalcRankScore takes it into a local before working through the same
 * record.
 *
 * Nothing in matched C sets it. The one writer, func_800218F0, is still
 * assembly and reaches the word with lw and sw, so where the pointer comes
 * from is not established here -- only what it points at. */
extern DuelResultDisplayState *D_8009B1E8;

#endif
