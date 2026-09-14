#ifndef MEMORIES_DECOMP_FUNC_8002A3CC_H
#define MEMORIES_DECOMP_FUNC_8002A3CC_H

#include "../types.h"
#include "display_object.h"
#include "../ygo_types.h"

/* The record itself, as its typed consumers spell it: func_80029590,
 * func_8002A3CC and func_8002A4A8 open with
 * `LibraryMotionState *state = &D_800EA1E8;`. The type and its offset asserts
 * live in ygo_types.h; only this typed declaration lives here.
 *
 * The same address is also declared as `u8 D_800EA1E8[]` in library_runtime.h.
 * func_8002BAB4 (src/game/library_runtime.c) reads only its first byte and
 * dispatches the library screen state on the low nibble.
 *
 * func_8002BFCC (src/game/func_8002BD0C.c) reaches nine distinct offsets
 * through that byte view: the mode byte, x and y at 0x08 and 0x0A, rest_x and
 * rest_y at 0x12 and 0x14, render at 0x44, and then 0x48, 0x54 and a per-card
 * sweep at 0x56 with a four-byte stride. It spells the five motion fields as
 * LibraryMotionState members through a cast of its byte pointer, which builds
 * byte-identical. The mode byte and everything at or past the asserted 0x48
 * size stay byte reaches. It can name the type without including this header
 * because the type is no longer defined beside this declaration.
 *
 * That it drives x, y, rest_x, rest_y and render is also evidence about the
 * open question below: those are this record's motion fields, not a mode
 * byte a neighbouring object might share, so the file is walking this record
 * rather than something that merely starts at the same address. What follows
 * 0x48 is a separate question, and the stride-4 sweep at 0x56 is the first
 * thing recorded about it.
 *
 * That byte view is not folded in on purpose. Whether the mode byte is a field
 * of this record or a separate object sharing its first bytes is not
 * established, and a typed declaration reaching those two files would assert
 * that it is. func_8002A4A8 shares this typed motion view, while two further
 * accessors are still assembly. */
extern LibraryMotionState D_800EA1E8;

s32 func_8002A3CC(void);
void func_8002A4A8(s32 column, s32 row, s32 frames);

#endif
