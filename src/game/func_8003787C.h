#ifndef MEMORIES_DECOMP_FUNC_8003787C_H
#define MEMORIES_DECOMP_FUNC_8003787C_H

#include "../types.h"
#include "duel_effect.h"

/* One D_80090E64 entry, the sibling of func_800378D8: same state_51/bit 0x80
 * gating, but it also runs func_80039FD4 on D_8009B328 before clearing state_51
 * when that record's display-effect step is zero. The callback table casts
 * this definition to its generic byte-pointer entry type. */
void func_8003787C(DuelEffectChannel *object);

#endif
