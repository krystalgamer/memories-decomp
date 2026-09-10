#ifndef MEMORIES_DECOMP_FUNC_800408D0_H
#define MEMORIES_DECOMP_FUNC_800408D0_H

#include "../types.h"
#include "display_object.h"

/* Submits one display object as a sprite in vertical strips of up to 64
 * pixels: `tex` is the object's texture word and `mode_arg` the halfword at
 * +0x14. func_80040BF8 (display_object_updates.c) calls it for every
 * renderable object in the update list.
 *
 * The third parameter is s32 and the definition narrows it to a u16 local.
 * The caller loads the halfword signed (lh), and a u16 parameter turns that
 * into lhu. The callee masks with andi 0xFFFF either way, so the narrowing
 * belongs on its side of the call. */
void func_800408D0(DisplayObject *e, s32 tex, s32 mode_arg);

#endif
