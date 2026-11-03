#ifndef MEMORIES_DECOMP_DISPLAY_ASSET_BANKS_H
#define MEMORIES_DECOMP_DISPLAY_ASSET_BANKS_H

#include "../types.h"

/* Reused display-resource bank. Transfer callbacks fill it, and resident
 * result/menu code and overlays pass it to DisplayObject_ConfigureSpriteAtPositionWithResource as the resource
 * argument. It is a byte-addressed bank, not a DisplayObject allocation;
 * the callers do not establish a common full extent. Nothing takes its
 * sizeof; an old [16] bound in DuelResult_UpdateOrbitSprite was decorative. */
extern u8 gDisplay_abResourceBank0[];

/* Second resource bank, with the same unsized-byte-array spelling in its
 * resident transfer and main-menu display users. Nothing takes its sizeof. */
extern u8 gDisplay_abResourceBank1[];

#endif
