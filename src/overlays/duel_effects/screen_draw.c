#include "../../types.h"
#include "../../game/graphics_constants.h"
#include "color_helpers.h"

void func_801556F4(u8 *color, u16 step)
{
    POLY_F4 polygon;

    setPolyF4(&polygon);
    setRGB0(&polygon, color[0], color[1], color[2]);
    setXY4(&polygon, 0, 0, GRAPHICS_DEFAULT_WIDTH, 0, 0, GRAPHICS_DEFAULT_HEIGHT,
           GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT);
    func_8005B260((u32 *)&polygon, D_8015B7F4, D_8015B800, 1);
    func_80153F28(color, step);
}

void func_80155798(u8 *color)
{
    POLY_G4 polygon;
    POLY_G4 *packet = &polygon;

    setPolyG4(packet);
    setRGB0(packet, 0, 0, 0);
    setRGB1(packet, 0, 0, 0);
    setRGB2(packet, color[0], color[1], color[2]);
    setRGB3(packet, color[0], color[1], color[2]);
    setXY4(packet, 0, 0, GRAPHICS_DEFAULT_WIDTH, 0, 0, GRAPHICS_DEFAULT_HEIGHT,
           GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT);
    func_8005B260((u32 *)packet, D_8015B7F4, D_8015B800, 1);
}

void func_80155864(u8 *color)
{
    POLY_F4 polygon;
    POLY_F4 *packet = &polygon;

    setPolyF4(packet);
    setRGB0(packet, color[0], color[1], color[2]);
    setXY4(packet, 0, 0, GRAPHICS_DEFAULT_WIDTH, 0, 0, GRAPHICS_DEFAULT_HEIGHT,
           GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT);
    func_8005B260((u32 *)packet, D_8015B7F4, D_8015B800, 1);
}
