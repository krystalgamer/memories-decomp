#ifndef MEMORIES_DECOMP_DUEL_EFFECT_PLAY_SOUND_COMMAND_H
#define MEMORIES_DECOMP_DUEL_EFFECT_PLAY_SOUND_COMMAND_H

#include "../types.h"
#include "duel_effect.h"

/* The three D_80090EAC sound commands. All read their operand through
 * func_80036D3C, which is the "next word of this object's current script
 * stream" accessor, so none of them touches the cursor arithmetic directly
 * except func_800386B8, which reads a raw opcode byte first.
 *
 * DuelEffect_PlaySoundCommand plays a sound effect or, with bit 15 set, routes
 * the id through func_8003FF88 and latches a second word into D_8009B33C.
 * func_80038690 is the plain BGM play. func_800386B8 is the BGM control byte:
 * a non-zero low six bits play the next stream word, otherwise bits 0, 1 and 2
 * replay, latch or copy the current command -- and bit 7 puts the object into
 * wait state 0xC and raises D_8009B350.
 *
 * func_80038690's parameter is `void *` in the definition where its two
 * siblings take u8 *; the table entry casts. */
void DuelEffect_PlaySoundCommand(DuelEffectChannel *object);
void func_80038690(void *object);
void func_800386B8(DuelEffectChannel *object);

#endif
