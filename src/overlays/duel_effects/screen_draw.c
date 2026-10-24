#include "../../types.h"
#include "color_semantic_views.h"
#include "../../game/graphics_constants.h"
#include "color_helpers.h"
#include "render_state_semantic_views.h"

void DuelEffect_DrawFadingFullscreenColor(u8 *color, u16 step)
{
    POLY_F4 polygon;

    setPolyF4(&polygon);
    setRGB0(&polygon, color[0], color[1], color[2]);
    setXY4(&polygon, 0, 0, GRAPHICS_DEFAULT_WIDTH, 0, 0, GRAPHICS_DEFAULT_HEIGHT,
           GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT);
    func_8005B260((u32 *)&polygon, gDuelEffect_pOrderingTable, gDuelEffect_wBasePriority, 1);
    DuelEffect_DarkenColor(color, step);
}

void DuelEffect_DrawFullscreenGradient(u8 *color)
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
    func_8005B260((u32 *)packet, gDuelEffect_pOrderingTable, gDuelEffect_wBasePriority, 1);
}

void DuelEffect_DrawFullscreenColor(u8 *color)
{
    POLY_F4 polygon;
    POLY_F4 *packet = &polygon;

    setPolyF4(packet);
    setRGB0(packet, color[0], color[1], color[2]);
    setXY4(packet, 0, 0, GRAPHICS_DEFAULT_WIDTH, 0, 0, GRAPHICS_DEFAULT_HEIGHT,
           GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT);
    func_8005B260((u32 *)packet, gDuelEffect_pOrderingTable, gDuelEffect_wBasePriority, 1);
}
