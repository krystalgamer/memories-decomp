#ifndef MEMORIES_DECOMP_CHECKERBOARD_BACKGROUND_H
#define MEMORIES_DECOMP_CHECKERBOARD_BACKGROUND_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

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
    /* None of the three is a scroll offset or phase. The tiler copies
       attribute into the sprite's GsSPRITE attribute word and rgb into its
       r/g/b word, and passes ot_depth to GsSortFastSprite as the ordering
       table priority. They sit at the offsets DisplayObject gives its own
       attribute, rgb and ot_depth, which fits a display object being
       handed in, but with no caller that is not proven. */
    u8 pad_00[4];
    s32 attribute;
    u8 pad_08[4];
    s32 rgb;
    u8 pad_10[4];
    s16 ot_depth;
} CheckerboardState;

/* PS1 scratchpad SPRT-primitive builder, reused for every sprite drawn. This
 * is the layout the tiler writes, not a Psy-Q type: it is filled by hand and
 * handed to the ordering table, so it is kept here rather than replaced with a
 * libgpu primitive. */
typedef struct {
    /* The first 0x18 bytes of a GsSPRITE, which is what every sprite is
       handed to GsSortFastSprite as, so the members take libgs.h's names
       (and SpritePrim's for the packed pairs): wh packs w in its low half
       and h in its high half, and uv's two bytes are u and v. */
    u32 attribute;
    u16 x;
    u16 y;
    u32 wh;
    u16 tpage;
    union {
        u16 word;
        struct {
            u8 u;
            u8 v;
        } bytes;
    } uv;
    u16 cx;
    u16 cy;
    u32 rgb;
} CheckerboardSprite;

void func_8003D32C(void);
void Checkerboard_DrawBackground(
    CheckerboardState *state, GsOT *ordering_table);

#endif
