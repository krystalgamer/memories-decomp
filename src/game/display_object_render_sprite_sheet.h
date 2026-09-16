#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_RENDER_SPRITE_SHEET_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_RENDER_SPRITE_SHEET_H

#include "../types.h"
#include "display_object.h"
#include "display_object_projection.h"

/* A display object's sprite sheet, reached through its +0x4C word: a
 * four-byte header followed by `count` part records. */
typedef struct {
    u8 count;
    u8 flags;      /* 0x10: 10-bit offsets; 0xE0: parts carry their own page */
    u8 tpage;      /* added to the object's +0x66 page, doubled when wide */
    u8 clut;       /* low nibble: clut x step (x16); high nibble: clut y step */
} SpriteSheetHeader;

/* One part of the sheet, six bytes. `cell` packs the texture position (bits
 * 0-4 u and 5-9 v, in units of 8), the part's page offset (bits 10-12), the
 * mirror bit 0x2000 and bits 8-9 of dx (bits 14-15). `size` packs the clut
 * step of paged parts (low bits), the width (bits 5-8) and height (bits
 * 9-12) as (n + 1) * 8, and bits 8-9 of dy (bits 14-15). */
typedef struct {
    s8 dx;
    s8 dy;
    u16 cell;
    u16 size;
} SpriteSheetPart;

/* Per-object working state DisplayObject_RenderSpriteSheet keeps in the
 * scratchpad at 0x1F800378. `proj` is the projection func_80041F90 fills and
 * func_80042188 takes as its origin record. */
typedef struct {
    u32 pad_00;
    u32 attribute;
    u16 cell;
    u16 size;
    u16 cx;
    u16 cy;
    u16 u;
    u16 v;
    s16 mx;
    s16 my;
    u16 tpage;
    u16 clip;
    s32 wide;
    struct ProjectionOut proj;
    s32 mode;
} SpriteSheetWork;

/* The rest of that state, at 0x1F8003A4. */
typedef struct {
    s32 pad_00;
    s32 mirror;
    s32 flip;
    s32 screen;
    s32 flags;
} SpriteSheetState;

/* Draws a display object whose image is a sprite sheet: every part becomes
 * one GsSPRITE-shaped primitive in the scratchpad at 0x1F800320, positioned
 * relative to the object and submitted through func_80042188. */
void DisplayObject_RenderSpriteSheet(DisplayObject *object, s32 ot, s32 depth);

#endif
