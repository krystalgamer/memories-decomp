#ifndef MEMORIES_DECOMP_SOUND_SECONDARY_OBJECT_SELECTION_H
#define MEMORIES_DECOMP_SOUND_SECONDARY_OBJECT_SELECTION_H

#include "../types.h"

/* Returns the index of the last eligible secondary object on channel `value`
 * (compared as its low byte) whose 16-bit priority at +0x1E is the smallest
 * seen, or -1 when none is eligible. The note starter func_8004ADE8, still a
 * candidate, calls it with the full-word channel. */
s32 func_8004A854(s32 value);

s32 func_8004A8E4(s32 index, s32 value);
s32 func_8004A940(s32 value, s32 variant);

#endif
