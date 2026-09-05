#include "../types.h"

extern s16 gGraphics_sViewportX;
extern s16 gGraphics_sViewportY;

void func_8002A660(u8 *arg0)
{
    s32 d = *(s16 *)(arg0 + 0xA) - gGraphics_sViewportY;
    s32 h;

    gGraphics_sViewportX = 0;
    h = *(u16 *)(arg0 + 0xA);
    if (d < 0x40) {
        gGraphics_sViewportY = h - 0x40;
    }
    if (d >= 0xB0) {
        gGraphics_sViewportY = *(u16 *)(arg0 + 0xA) - 0xB0;
    }
}
