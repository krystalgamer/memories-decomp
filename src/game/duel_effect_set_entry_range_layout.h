#ifndef MEMORIES_DECOMP_DUEL_EFFECT_SET_ENTRY_RANGE_LAYOUT_H
#define MEMORIES_DECOMP_DUEL_EFFECT_SET_ENTRY_RANGE_LAYOUT_H

#include "../types.h"

void DuelEffect_SetEntryRangeLayout(s32 range, s32 layout);

/* Compatibility symbol used by existing resident and overlay callers. */
void func_8003B6AC(s32 range, s32 layout);

#endif
