#ifndef MEMORIES_DECOMP_MODEL_SLOT_UPDATES_H
#define MEMORIES_DECOMP_MODEL_SLOT_UPDATES_H

#include "../types.h"
#include "model.h"

/* The two per-slot update steps model_slot_setup.c and
 * func_80059700 (src/game/func_80059700.c) drive. func_8005A468 sets
 * the playback speed of every part's GsSEQ in one slot; func_8005A4C4 takes
 * the slot record itself rather than its index, which is why the two do not
 * share a parameter shape. */
void func_8005A468(s32 index, s32 speed);
void func_8005A4C4(ModelSlot *record, s32 arg1, s32 arg2, s32 arg3, s32 arg4);

#endif
