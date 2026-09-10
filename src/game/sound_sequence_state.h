#ifndef MEMORIES_DECOMP_SOUND_SEQUENCE_STATE_H
#define MEMORIES_DECOMP_SOUND_SEQUENCE_STATE_H

#include "../types.h"

/* The two sequence-state setters. Both write g_SDValue's field_1582 and
 * field_1584 from their arguments, and their bodies are identical -- retail
 * has two symbols for the same two stores, and sound_output.c calls one on
 * each side of a branch. They are declared separately here because they are
 * separate symbols, not because anything distinguishes them yet. */
void func_800490F0(s16 value, u8 flag);
void func_80049108(s16 value, u8 flag);

/* The state query reads only g_SDValue. func_800379C4 still passes its effect
 * object in $a0, matching the retail caller sequence. */
#ifdef FUNC_80049120_IGNORES_OBJECT
s32 func_80049120(void *object);
#else
s32 func_80049120(void);
#endif

#endif
