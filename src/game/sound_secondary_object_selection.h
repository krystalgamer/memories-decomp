#ifndef MEMORIES_DECOMP_SOUND_SECONDARY_OBJECT_SELECTION_H
#define MEMORIES_DECOMP_SOUND_SECONDARY_OBJECT_SELECTION_H

#include "../types.h"

/* Releases a secondary object record for reuse and returns its index. */
s32 func_8004A8E4(s32 index, s32 value);

/* Selects or reuses a secondary object for a value/variant pair. */
s32 func_8004A940(s32 value, s32 variant);

#endif
