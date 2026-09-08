#include "../../types.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_constants.h"
#include "frontend.h"

extern u8 D_80184597;
extern GsOT *D_800E9D90[];

void MainMenu_DrawFrontendBackground(void)
{
    POLY_F4 flat;
    POLY_FT4 sprite;
    POLY_G4 shade;
    s32 shadeLevel;
    s32 x;
    s32 right;
    s32 u;

    shadeLevel = D_80184597;
    if (shadeLevel != 0) {
        setPolyF4(&flat);
        flat.r0 = shadeLevel;
        flat.g0 = shadeLevel;
        flat.b0 = shadeLevel;
        flat.x0 = 0;
        flat.y0 = 0;
        flat.x1 = GRAPHICS_DEFAULT_WIDTH;
        flat.y1 = 0;
        flat.x2 = 0;
        flat.y2 = GRAPHICS_DEFAULT_HEIGHT;
        flat.x3 = GRAPHICS_DEFAULT_WIDTH;
        flat.y3 = GRAPHICS_DEFAULT_HEIGHT;
        func_8005B260((u32 *)&flat, (GsOT *)D_800E9D90[2], 0, 2);
    }
    setPolyFT4(&sprite);
    sprite.r0 = 128;
    sprite.g0 = 128;
    sprite.b0 = 128;
    sprite.tpage = 15;
    sprite.clut = getClut(0, 244);
    for (x = 0; x < GRAPHICS_DEFAULT_WIDTH; x = right) {
        u = x % 256;
        right = x + 64;
        sprite.x0 = x;
        sprite.y0 = 0;
        sprite.x1 = right;
        sprite.y1 = 0;
        sprite.x2 = x;
        sprite.y2 = GRAPHICS_DEFAULT_HEIGHT;
        sprite.x3 = right;
        sprite.y3 = GRAPHICS_DEFAULT_HEIGHT;
        sprite.u0 = u;
        sprite.v0 = 0;
        sprite.u1 = u + 63;
        sprite.v1 = 0;
        sprite.u2 = u;
        sprite.v2 = 239;
        sprite.u3 = u + 63;
        sprite.v3 = 239;
        GsSortPoly(&sprite, D_800E9D90[2], 4095);
    }
    setPolyG4(&shade);
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
    shade.x1 = GRAPHICS_DEFAULT_WIDTH;
    shade.y1 = 0;
    shade.x2 = 0;
    shade.y2 = GRAPHICS_DEFAULT_HEIGHT;
    shade.x3 = GRAPHICS_DEFAULT_WIDTH;
    shade.y3 = GRAPHICS_DEFAULT_HEIGHT;
    func_8005B260((u32 *)&shade, (GsOT *)D_800E9D90[2], 4094, 2);
}
