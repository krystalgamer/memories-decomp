#ifndef MEMORIES_DECOMP_FUNC_80037C74_H
#define MEMORIES_DECOMP_FUNC_80037C74_H

#include "../types.h"

/* Advances an object's scroll position by one step and reports whether it has
 * reached the end.
 *
 * When the counter at 0x38 reaches its limit at 0x3E the counter resets and
 * the position at 0x3A moves on by the step held at 0x5B. If that would carry
 * the position past the bound at 0x42 the step is taken back off and 1 is
 * returned, leaving the position at the last value that still fits.
 *
 * The return is a flag, not a count. */
s32 func_80037C74(u8 *object);

#endif
