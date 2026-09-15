#include "../types.h"
#include "../ygo_types.h"
#include "graphics_frame.h"
#include "func_8002A660.h"

void func_8002A660(u8 *record)
{
    s32 d =
        ((LibraryMotionState *)record)->y - gGraphics_sViewportY;
    s32 h;

    gGraphics_sViewportX = 0;
    h = (u16)((LibraryMotionState *)record)->y;
    if (d < 0x40) {
        gGraphics_sViewportY = h - 0x40;
    }
    if (d >= 0xB0) {
        gGraphics_sViewportY =
            (u16)((LibraryMotionState *)record)->y - 0xB0;
    }
}
