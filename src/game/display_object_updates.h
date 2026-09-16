#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_UPDATES_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_UPDATES_H

#include "../types.h"
#include "display_object.h"

/* Submits one display object as a sprite in vertical strips of up to 64
 * pixels: `tex` is the object's texture word and `mode_arg` the halfword at
 * +0x14. DisplayObject_RenderSpriteStripList below is its only caller, and
 * reaches it for every renderable object in the update list.
 *
 * The third parameter is s32 and the definition narrows it to a u16 local.
 * The caller loads the halfword signed (lh), and a u16 parameter turns that
 * into lhu. The callee masks with andi 0xFFFF either way, so the narrowing
 * belongs on its side of the call. */
void func_800408D0(DisplayObject *e, s32 tex, s32 mode_arg);

/* Three of the seven entries in gDisplayObject_ListRenderers. Each walks one
 * display object list from its head in D_800EFE38, runs every object's update
 * callback, and differs only in which list it takes and what it does afterwards:
 *
 *   DisplayObject_RunUpdateCallbackList  list 0, callback-only
 *   DisplayObject_RenderSpriteStripList  list 3
 *   func_80040D14  list 6
 *
 * The list-3 walker submits sprite strips, and the list-6 walker invokes the
 * object's second callback slot at +0x4C. All three are reached only as
 * entries of that table, so the table file is their only consumer and
 * declared all three itself before this header. */
void DisplayObject_RenderSpriteStripList(void);
void DisplayObject_RunUpdateCallbackList(void);
void func_80040D14(void);

#endif
