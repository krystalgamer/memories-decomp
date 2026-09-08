#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "trade_helpers.h"

extern GsOT *D_800E9D90[];

void MainMenu_DrawThreeDigitNumber(s32 x, s32 y, s32 value)
{
    POLY_FT4 sprite;
    s32 i;
    s32 quotient;
    s32 digit;
    s32 base;
    s32 left;
    s32 right;
    s32 u;
    s32 top;
    s32 bottom;

    i = 0;
    top = y + 4;
    bottom = y + 12;
    setPolyFT4(&sprite);
    sprite.tpage = 11;
    sprite.clut = 0x3EA9;
    sprite.r0 = 255;
    sprite.g0 = 255;
    sprite.b0 = 255;
    for (; i < 3; i++) {
        base = x + 24 - i * 8;
        left = base - 12;
        sprite.x0 = left;
        right = base - 4;
        sprite.x1 = right;
        sprite.x3 = right;
        quotient = value / 10;
        sprite.y0 = top;
        sprite.y1 = top;
        sprite.x2 = left;
        sprite.y2 = bottom;
        sprite.y3 = bottom;
        sprite.v0 = 112;
        sprite.v1 = 112;
        sprite.v2 = 120;
        sprite.v3 = 120;
        digit = value - quotient * 10;
        u = digit * 8;
        sprite.u0 = u - 128;
        sprite.u1 = u - 120;
        sprite.u2 = u - 128;
        sprite.u3 = u - 120;
        GsSortPoly(&sprite, D_800E9D90[1], 32);
        value = quotient;
    }
}
