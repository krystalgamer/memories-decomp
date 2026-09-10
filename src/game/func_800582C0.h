#ifndef MEMORIES_DECOMP_FUNC_800582C0_H
#define MEMORIES_DECOMP_FUNC_800582C0_H

#include "../types.h"

/* Recolours one duel side's 256x8 VRAM strip while copying it up from y=0xF8
 * to y=0xF0. side selects the half of the framebuffer through `side << 8`, so
 * 0 reads x=0 and 1 reads x=256, and the function returns without drawing for
 * any other value.
 *
 * The slow path walks the strip in two 256x4 bands: StoreImage2 pulls a band
 * into a local buffer, every one of its 0x400 pixels goes through
 * func_8005AE68(pixel, tint, level), and LoadImage2 writes the result back
 * eight rows higher. When the tint keeps the hue, is not inverted, and level
 * is at or above COLOR_FIXED_ONE the recolour is the identity, so the whole
 * strip moves in one MoveImage instead.
 *
 * model_slot_setup.c is the only caller and supplies the tint and level from
 * the slot's field_E0C and field_E0A. */
void func_800582C0(s32 side, s32 tint, s32 level);

#endif
