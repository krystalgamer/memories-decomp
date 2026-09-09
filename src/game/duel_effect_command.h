#ifndef MEMORIES_DECOMP_DUEL_EFFECT_COMMAND_H
#define MEMORIES_DECOMP_DUEL_EFFECT_COMMAND_H

#include "../types.h"

/* The shared step every handler in this unit runs: it raises bit 0x80 of the
 * halfword at 0x34, calls func_80036C14 with the value it was given, lowers
 * the bit again and advances the halfword at 0x38 by 0x10. It is not in the
 * command table itself -- the four handlers below are its only callers. */
void func_80038024(u8 *object, s32 value);

/* Takes void *, not u8 *. It forwards its argument straight to
 * func_80038024 and never indexes it, so the definition never needed the
 * byte type. The command table stores it in an array of u8 * handlers and
 * casts it there. */
void func_80038070(void *object);

void func_80038094(u8 *object);
void func_800380D4(u8 *object);
void func_80038110(u8 *object);

#endif
