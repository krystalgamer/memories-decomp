#ifndef MEMORIES_DECOMP_SPRITE_PRIMITIVE_H
#define MEMORIES_DECOMP_SPRITE_PRIMITIVE_H

#include "../types.h"

/* The sprite primitive both display-object renderers build in the scratchpad
   at 0x1F800320 before handing it to func_80042188.
 
   It is a GsSPRITE (libgs.h) field for field, and the layout is not the only
   evidence for that. display_object_core.c initialises the object fields
   these are copied from with 0x00808080 and 0x10001000, which are r/g/b at
   0x80 each and scalex/scaley at 1.0 in 4.12; func_800408D0 advances `tpage`
   by a page step and bounds `uv.b.lo` at 0x100, which is what a u coordinate
   does; and func_80040588 writes `rotate` only when the attribute's GsROTOFF
   bit is clear.
 
     0x00 attribute   0x0C tpage       0x18 mx, my
     0x04 x, y        0x0E u, v        0x1C scalex, scaley
     0x08 w, h        0x10 cx, cy      0x20 rotate
                      0x14 r, g, b
 
   It is spelt here rather than reused from libgs.h because the renderers
   reach several of these pairs as whole words - `xy`, `mxmy` and the `w`/`h`
   pair are each copied from the display object in one 32-bit move - while
   GsSPRITE spells every one of them as two separate members. The unions
   below record both widths instead of forcing either. */

/* Two 16-bit halves of a word, reached either way. */
typedef union {
    s32 word;
    struct {
        u16 x;
        u16 y;
    } h;
} SpritePos;

/* Two 8-bit halves of a halfword, reached either way. */
typedef union {
    u16 word;
    struct {
        u8 lo;
        u8 hi;
    } b;
} SpriteHalf;

typedef struct {
    u32 attribute;              /* 0x00 */
    SpritePos xy;               /* 0x04 */
    /* func_80040588 copies w and h together out of the object's 0x3C word;
       func_800408D0 sets h once and then rewrites w per strip, reading its
       low byte as the u advance. */
    union {
        u32 word;
        struct {
            SpriteHalf w;       /* 0x08 */
            u16 h;              /* 0x0A */
        } wh;
    } extent;                   /* 0x08 */
    u16 tpage;                  /* 0x0C */
    SpriteHalf uv;              /* 0x0E */
    /* GsSPRITE's cx/cy, the CLUT position. The two display-object renderers
       copy it out of the object's 0x40 word whole; func_80016784 sets the
       halves separately and steps cy by one per spell-frame class. */
    union {
        u32 word;
        struct {
            u16 cx;             /* 0x10 */
            u16 cy;             /* 0x12 */
        } h;
    } cxcy;                     /* 0x10 */
    u32 rgb;                    /* 0x14 */
    SpritePos mxmy;             /* 0x18 */
    u32 scale;                  /* 0x1C */
    s32 rotate;                 /* 0x20 */
} SpritePrim;

typedef char SpritePrim_size_must_be_0x24[
    sizeof(SpritePrim) == 0x24 ? 1 : -1
];
typedef char SpritePrim_cy_must_be_at_0x12[
    (u32)&(((SpritePrim *)0)->cxcy.h.cy) == 0x12 ? 1 : -1
];

/* The renderer the display-object paths hand the finished primitive to.
   It was declared privately and identically in the three src/game/ sources
   that call it, all of which already include this header. */
void func_80042188(SpritePrim *arg0, u8 *arg1, s32 arg2, s32 arg3, u8 *arg4);

/* The scratchpad block at 0x1F800378 that func_80041F90 writes its clip
   result into. */
typedef struct {
    u32 unk0;
    u32 flag;
    u8 pad8[0x18];
    u8 out[4];
} ClipState;

#endif
