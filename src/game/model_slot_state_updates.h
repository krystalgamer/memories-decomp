#ifndef MEMORIES_DECOMP_MODEL_SLOT_STATE_UPDATES_H
#define MEMORIES_DECOMP_MODEL_SLOT_STATE_UPDATES_H

#include "../types.h"

/* Two updates applied to one model slot, both selected by slot index rather
 * than by a slot pointer, so callers do not need the record's layout.
 *
 * func_80059700 turns a sign into the slot's velocity; func_800597C8 sets the
 * slot's pending animation. */
void func_80059700(s32 index, s32 sign);
void func_800597C8(s32 idx, s32 flag, s32 val);

#endif
