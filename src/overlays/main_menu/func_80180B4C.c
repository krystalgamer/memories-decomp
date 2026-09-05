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
    s16 x1;
    s16 y1;
    s16 x2;
    s16 y2;
    s16 x3;
    s16 y3;
} MainMenuFlatQuad;

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

typedef struct {
    u8 pad[3];
    u8 len;
    u8 r0;
    u8 g0;
    u8 b0;
    u8 code;
    s16 x0;
    s16 y0;
    u8 r1;
    u8 g1;
    u8 b1;
    u8 pad1;
    s16 x1;
    s16 y1;
    u8 r2;
    u8 g2;
    u8 b2;
    u8 pad2;
    s16 x2;
    s16 y2;
    u8 r3;
    u8 g3;
    u8 b3;
    u8 pad3;
    s16 x3;
    s16 y3;
} MainMenuGouraudQuad;

extern u8 D_80184597;
extern void *D_800E9D90[];
extern void func_8005B260(void *, void *, s32, s32);
extern void func_80084320(void *, void *, s32);

void func_80180B4C(void)
{
    MainMenuFlatQuad flat;
    MainMenuSprite sprite;
    MainMenuGouraudQuad shade;
    s32 shadeLevel;
    s32 x;
    s32 right;
    s32 u;

    shadeLevel = D_80184597;
    if (shadeLevel != 0) {
        flat.len = 5;
        flat.r = shadeLevel;
        flat.g = shadeLevel;
        flat.b = shadeLevel;
        flat.code = 0x28;
        flat.x0 = 0;
        flat.y0 = 0;
        flat.x1 = 320;
        flat.y1 = 0;
        flat.x2 = 0;
        flat.y2 = 240;
        flat.x3 = 320;
        flat.y3 = 240;
        func_8005B260(&flat, D_800E9D90[2], 0, 2);
    }
    sprite.len = 9;
    sprite.code = 0x2C;
    sprite.r = 128;
    sprite.g = 128;
    sprite.b = 128;
    sprite.tpage = 15;
    sprite.clut = 0x3D00;
    for (x = 0; x < 320; x = right) {
        u = x % 256;
        right = x + 64;
        sprite.x0 = x;
        sprite.y0 = 0;
        sprite.x1 = right;
        sprite.y1 = 0;
        sprite.x2 = x;
        sprite.y2 = 240;
        sprite.x3 = right;
        sprite.y3 = 240;
        sprite.u0 = u;
        sprite.v0 = 0;
        sprite.u1 = u + 63;
        sprite.v1 = 0;
        sprite.u2 = u;
        sprite.v2 = 239;
        sprite.u3 = u + 63;
        sprite.v3 = 239;
        func_80084320(&sprite, D_800E9D90[2], 4095);
    }
    shade.len = 8;
    shade.code = 0x38;
    shade.r2 = 255;
    shade.g2 = 255;
    shade.b2 = 255;
    shade.r3 = 255;
    shade.g3 = 255;
    shade.b3 = 255;
    shade.r0 = 0;
    shade.g0 = 0;
    shade.b0 = 0;
    shade.r1 = 0;
    shade.g1 = 0;
    shade.b1 = 0;
    shade.x0 = 0;
    shade.y0 = 0;
    shade.x1 = 320;
    shade.y1 = 0;
    shade.x2 = 0;
    shade.y2 = 240;
    shade.x3 = 320;
    shade.y3 = 240;
    func_8005B260(&shade, D_800E9D90[2], 4094, 2);
}
