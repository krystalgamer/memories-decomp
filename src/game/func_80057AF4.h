#ifndef MEMORIES_DECOMP_FUNC_80057AF4_H
#define MEMORIES_DECOMP_FUNC_80057AF4_H

#include "../types.h"

/* Starts an animation on one of the D_800F2C40 slots. `index` picks the slot,
 * at a stride of 0xE20, and `anim` picks an entry within it.
 *
 * Two things a prototype cannot say. It may do nothing at all: an entry whose
 * length is zero returns immediately, and so does a slot that already has an
 * animation running when `flag` is zero. And `flag` only means anything in
 * that second case -- when something is already playing it commits the queued
 * animation and re-enters as (index, 0, 0), which is why the defining file
 * needs this declaration before the definition rather than only its callers.
 *
 * All three callers already spelled it exactly this way, as did the file's own
 * forward declaration. */
void func_80057AF4(s32 index, s32 anim, s32 flag);

#endif
