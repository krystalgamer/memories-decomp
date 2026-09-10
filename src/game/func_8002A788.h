#ifndef MEMORIES_DECOMP_FUNC_8002A788_H
#define MEMORIES_DECOMP_FUNC_8002A788_H

#include "../types.h"

/* State 1 of the library screen dispatcher, handed the same record the other
 * states take -- func_8002BAB4.c's switch calls it for mode 1 alongside
 * func_8002BAA0 and func_8002BAAC.
 *
 * The unit implements it as a top-level asm block rather than as C, so this
 * prototype is what the compiler sees on both sides; the block's own
 * `.reloc .-4, R_MIPS_26, func_8002A788` resolves the call.
 *
 * func_8002A6B8, the other function this unit defines, is deliberately absent.
 * It is defined `s32 func_8002A6B8(void)` here and declared
 * `s32 func_8002A6B8(u8 *arg0)` in func_8002A2F4.c, which calls it with an
 * argument the definition does not name. A shared prototype would have to pick
 * one of those, and whether the extra argument is load bearing at that call has
 * not been measured -- the same shape as the func_8004CB0C case unmatched.h
 * records, where the caller's register was already live. It needs its own
 * change. */
void func_8002A788(u8 *state);

#endif
