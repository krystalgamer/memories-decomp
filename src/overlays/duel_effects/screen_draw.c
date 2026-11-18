#include "../../types.h"
#include "color_semantic_views.h"
#include "../../game/graphics_constants.h"
#include "color_helpers.h"
#include "render_state_semantic_views.h"

void DuelEffect_DrawFadingFullscreenColor(CVECTOR *color, u16 step)
{
    POLY_F4 polygon;

    setPolyF4(&polygon);
    setRGB0(&polygon, color->r, color->g, color->b);
    setXY4(&polygon, 0, 0, GRAPHICS_DEFAULT_WIDTH, 0, 0, GRAPHICS_DEFAULT_HEIGHT,
           GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT);
    Graphics_SubmitDrawModePacket(&polygon, gDuelEffect_pOrderingTable, gDuelEffect_wBasePriority, 1);
    DuelEffect_DarkenColor(color, step);
}

void DuelEffect_DrawFullscreenGradient(CVECTOR *color)
{
    POLY_G4 polygon;
    POLY_G4 *packet = &polygon;

    setPolyG4(packet);
    setRGB0(packet, 0, 0, 0);
    setRGB1(packet, 0, 0, 0);
    setRGB2(packet, color->r, color->g, color->b);
    setRGB3(packet, color->r, color->g, color->b);
    setXY4(packet, 0, 0, GRAPHICS_DEFAULT_WIDTH, 0, 0, GRAPHICS_DEFAULT_HEIGHT,
           GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT);
    Graphics_SubmitDrawModePacket(packet, gDuelEffect_pOrderingTable, gDuelEffect_wBasePriority, 1);
}

void DuelEffect_DrawFullscreenColor(CVECTOR *color)
{
    POLY_F4 polygon;
    POLY_F4 *packet = &polygon;

    setPolyF4(packet);
    setRGB0(packet, color->r, color->g, color->b);
    setXY4(packet, 0, 0, GRAPHICS_DEFAULT_WIDTH, 0, 0, GRAPHICS_DEFAULT_HEIGHT,
           GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT);
    Graphics_SubmitDrawModePacket(packet, gDuelEffect_pOrderingTable, gDuelEffect_wBasePriority, 1);
}
