#ifndef MEMORIES_DECOMP_DUEL_EFFECT_COMMAND_H
#define MEMORIES_DECOMP_DUEL_EFFECT_COMMAND_H

#include "../types.h"
#include "duel_effect.h"

/* The shared step every handler in this unit runs: it raises bit 0x80 of the
 * halfword at 0x34, calls func_80036C14 with the value it was given, lowers
 * the bit again and advances the halfword at 0x38 by 0x10. It is not in the
 * command table itself -- the four handlers below are its only callers. */
void func_80038024(DuelEffectChannel *object, s32 value);

/* Forwards its argument straight to func_80038024 and never indexes
 * it, which is why the definition took void * before this record was
 * named. The command table stores it in an array of u8 * handlers and
 * casts it there, as it now does for the other three. */
void func_80038070(DuelEffectChannel *object);

void func_80038094(DuelEffectChannel *object);
void func_800380D4(DuelEffectChannel *object);
void func_80038110(DuelEffectChannel *object);

#endif
