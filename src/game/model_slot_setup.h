#ifndef MEMORIES_DECOMP_MODEL_SLOT_SETUP_H
#define MEMORIES_DECOMP_MODEL_SLOT_SETUP_H

#include "../types.h"

/* Duel-side layout pass for one player's model slot: sums the hand's card
 * widths (0x14 for the cards flagged in the +0xBEC bitfield, 0xC otherwise),
 * derives the two cursor limits at +0xDF0/+0xDF4, resets each card object's
 * sprite fields, and applies the mode-dependent horizontal offset through
 * func_8005A468 before func_800582C0 draws it.
 *
 * arg1 really is a pointer -- the body null-tests it as `arg1 == (u8 *)0`
 * before dereferencing. Both callers hold the value in D_80010000 and used to
 * declare the parameter as an integer; they now cast at the call instead. */
void func_80056250(s32 arg0, u8 *arg1, s32 arg2, s32 arg3);

#endif
