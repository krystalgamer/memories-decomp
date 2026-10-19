#ifndef MEMORIES_DECOMP_DUEL_EFFECT_CLEAR_RESOURCE_OBJECT_POINTERS_H
#define MEMORIES_DECOMP_DUEL_EFFECT_CLEAR_RESOURCE_OBJECT_POINTERS_H

#include "../types.h"

/* Clears both stored object pointers in one gCardPreview_Resources resource slot without
 * releasing them. Takes the slot index, not a record pointer. */
void DuelEffect_ClearResourceObjectPointers(s32 index);

#endif
