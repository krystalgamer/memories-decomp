#ifndef MEMORIES_DECOMP_FUNC_80029574_H
#define MEMORIES_DECOMP_FUNC_80029574_H

#include "../types.h"

/* Clears both stored object pointers in one D_800EA0E8 resource slot without
 * releasing them. Takes the slot index, not a record pointer. */
void DuelEffect_ClearResourceObjectPointers(s32 index);

#endif
