#include "../../types.h"

typedef struct {
    u8 pad[3];
    u8 len;
    u8 r;
    u8 g;
    u8 b;
    u8 code;
    s16 x0;
    s16 y0;
    u8 u0;
    u8 v0;
    u16 clut;
    s16 x1;
    s16 y1;
    u8 u1;
    u8 v1;
    u16 tpage;
    s16 x2;
    s16 y2;
    u8 u2;
    u8 v2;
    u16 pad2;
    s16 x3;
    s16 y3;
    u8 u3;
    u8 v3;
    u16 pad3;
} MainMenuSprite;

extern void *D_800E9D90[];
extern void func_80084320(void *, void *, s32);

void func_80183E8C(s32 x, s32 y, s32 value)
{
    MainMenuSprite sprite;
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
    sprite.len = 9;
    sprite.code = 0x2C;
    sprite.tpage = 11;
    sprite.clut = 0x3EA9;
    sprite.r = 255;
    sprite.g = 255;
    sprite.b = 255;
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
        func_80084320(&sprite, D_800E9D90[1], 32);
        value = quotient;
    }
}
