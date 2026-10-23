#ifndef MEMORIES_DECOMP_MODEL_APPLY_TEXTURE_TINT_H
#define MEMORIES_DECOMP_MODEL_APPLY_TEXTURE_TINT_H

#include "../types.h"

/* Recolours one duel side's 256x8 VRAM strip while copying it up from y=0xF8
 * to y=0xF0. side selects the half of the framebuffer through `side << 8`, so
 * 0 reads x=0 and 1 reads x=256, and the function returns without drawing for
 * any other value.
 *
 * The slow path walks the strip in two 256x4 bands: StoreImage2 pulls a band
 * into a local buffer, every one of its 0x400 pixels goes through
 * Color_TintBgr555Pixel(pixel, tint, level), and LoadImage2 writes the
 * result back eight rows higher. When the tint keeps the hue, is not
 * inverted, and level is at or above COLOR_FIXED_ONE the recolour is the
 * identity apart from the tint's zero-channel clamp, so the whole strip moves
 * in one MoveImage instead; notes/color-transform-runtime.md measures that
 * difference.
 *
 * Its RECT scratch is the four halfwords gModel_sTextureTintRectX/Y/W/H,
 * declared centrally in unmatched.h; this function is their only user.
 *
 * model_slot_control.c is the only caller and supplies the tint and level from
 * the slot's texture_tint_control and texture_tint_scale. */
void Model_ApplyTextureTint(s32 side, s32 tint, s32 level);

#endif
