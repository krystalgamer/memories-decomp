#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_RUNTIME_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_RUNTIME_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "display_object_projection.h"

/* Submits one display object as a sprite in vertical strips of up to 64
 * pixels: `ordering_table` is the selected GsOT descriptor, not a texture.
 * `mode_arg` carries the halfword at
 * +0x14. DisplayObject_RenderSpriteStripList below is its only caller, and
 * reaches it for every renderable object in the update list.
 *
 * The third parameter is s32 and the definition narrows it to a u16 local.
 * The caller loads the halfword signed (lh), and a u16 parameter turns that
 * into lhu. The callee masks with andi 0xFFFF either way, so the narrowing
 * belongs on its side of the call. */
void DisplayObject_RenderSpriteStrips(DisplayObject *e, GsOT *ordering_table, s32 mode_arg);

/* Three of the seven entries in gDisplayObject_ListRenderers. Each walks one
 * display object list from its head in gDisplayObject_awListHeads, runs every object's update
 * callback, and differs only in which list it takes and what it does afterwards:
 *
 *   DisplayObject_RunUpdateCallbackList  list 0, callback-only
 *   DisplayObject_RenderSpriteStripList  list 3
 *   DisplayObject_RunSecondaryCallbackList  list 6
 *
 * The list-3 walker submits sprite strips, and the list-6 walker invokes the
 * object's second callback slot at +0x4C. All three are reached only as
 * entries of that table, so the table file is their only consumer and
 * declared all three itself before this header. */
void DisplayObject_RenderSpriteStripList(void);
void DisplayObject_RunUpdateCallbackList(void);
void DisplayObject_RunSecondaryCallbackList(void);

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
 * scratchpad at 0x1F800378. `proj` is the projection
 * DisplayObject_ProjectRotationFacing fills and DisplayObject_SubmitPacket
 * takes as its origin record. */
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
 * relative to the object and submitted through DisplayObject_SubmitPacket. */
void DisplayObject_RenderSpriteSheet(DisplayObject *object, GsOT *ordering_table, s32 depth);

#endif
