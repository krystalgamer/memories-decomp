#include "../types.h"
#include "../ygo_types.h"
#include "graphics_frame.h"
#include "func_8002A660.h"

/* Regional value: the lower bound past which the viewport follows the motion
 * (0xB0 -> 0xC0). The European build (src/game/european/) defines its own. */
#ifndef LIBRARY_VIEWPORT_FOLLOW_BOTTOM
#define LIBRARY_VIEWPORT_FOLLOW_BOTTOM 0xB0
#endif

void func_8002A660(u8 *record)
{
    LibraryMotionState *motion;
    s32 d =
        ((LibraryMotionState *)record)->y - gGraphics_sViewportY;
    s32 h;

    motion = (LibraryMotionState *)record;
    gGraphics_sViewportX = 0;
    h = (u16)motion->y;
    if (d < 0x40) {
        gGraphics_sViewportY = h - 0x40;
    }
    if (d >= LIBRARY_VIEWPORT_FOLLOW_BOTTOM) {
        gGraphics_sViewportY = (u16)motion->y - LIBRARY_VIEWPORT_FOLLOW_BOTTOM;
    }
}
