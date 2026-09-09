#ifndef MEMORIES_DECOMP_CHECKERBOARD_BACKGROUND_H
#define MEMORIES_DECOMP_CHECKERBOARD_BACKGROUND_H

#include "../types.h"

/* The scrolling checkerboard backdrop: one empty entry point and the tiler
 * that fills the screen with alternating cells.
 *
 * Neither function has a caller. Nothing in src/ names them, and the only
 * references in the generated assembly are inside their own object, so the
 * two records below are described entirely by this translation unit. */

/* The state func_8003D334 reads its scroll offsets and phase from. Only the
 * three fields it touches are named; the gaps are padding to hold the offsets,
 * not evidence that the record ends at 0x16. */
typedef struct {
    u8 pad_00[4];
    s32 field_04;
    u8 pad_08[4];
    s32 field_0C;
    u8 pad_10[4];
    s16 field_14;
} CheckerboardState;

/* PS1 scratchpad SPRT-primitive builder, reused for every sprite drawn. This
 * is the layout the tiler writes, not a Psy-Q type: it is filled by hand and
 * handed to the ordering table, so it is kept here rather than replaced with a
 * libgpu primitive. */
typedef struct {
    u32 field_00;
    u16 field_04;
    u16 field_06;
    u32 field_08;
    u16 field_0C;
    u8 field_0E;
    u8 field_0F;
    u16 field_10;
    u16 field_12;
    u32 field_14;
} CheckerboardSprite;

void func_8003D32C(void);
void func_8003D334(CheckerboardState *state, void *ordering_table);

#endif
