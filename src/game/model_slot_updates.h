#ifndef MEMORIES_DECOMP_MODEL_SLOT_UPDATES_H
#define MEMORIES_DECOMP_MODEL_SLOT_UPDATES_H

#include "../types.h"

/* The two per-slot update steps model_slot_setup.c and
 * model_slot_state_updates.c drive. func_8005A468 applies one slot's velocity;
 * func_8005A4C4 takes the slot record itself rather than its index, which is
 * why the two do not share a parameter shape. */
void func_8005A468(s32 index, s32 velocity);
void func_8005A4C4(u8 *record, s32 arg1, s32 arg2, s32 arg3, s32 arg4);

#endif
