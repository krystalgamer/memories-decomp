#ifndef MEMORIES_DECOMP_FUNC_80021480_H
#define MEMORIES_DECOMP_FUNC_80021480_H

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
       as one group -- 0x44, a variant selector, and 0x45. The third
       was padding here because only func_80021480 had been read. */
    u8 text_styles[3];
    u8 pad_37;
} DuelResultDisplayState;

typedef char DuelResultDisplayState_children_offset_must_be_0x4[
    DUEL_RESULT_DISPLAY_OFFSET(children) == 0x4 ? 1 : -1
];
typedef char DuelResultDisplayState_text_styles_offset_must_be_0x34[
    DUEL_RESULT_DISPLAY_OFFSET(text_styles) == 0x34 ? 1 : -1
];
typedef char DuelResultDisplayState_size_must_be_0x38[
    sizeof(DuelResultDisplayState) == 0x38 ? 1 : -1
];

#undef DUEL_RESULT_DISPLAY_OFFSET

void func_80021480(s32 mode);

#endif
