#ifndef MEMORIES_DECOMP_FUNC_80029574_H
#define MEMORIES_DECOMP_FUNC_80029574_H

#include "../types.h"

/* Clears one of the D_800EA0E8 effect-resource slots' pending request. Takes the
 * slot index, not a record pointer. */
void DuelEffect_ClearResourceObjectPointers(s32 index);

#endif
