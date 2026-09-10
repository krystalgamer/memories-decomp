#ifndef MEMORIES_DECOMP_SOUND_SEQUENCE_STATE_H
#define MEMORIES_DECOMP_SOUND_SEQUENCE_STATE_H

#include "../types.h"

/* The state query reads only g_SDValue. func_800379C4 still passes its effect
 * object in $a0, matching the retail caller sequence. */
#ifdef FUNC_80049120_IGNORES_OBJECT
s32 func_80049120(void *object);
#else
s32 func_80049120(void);
#endif

#endif
