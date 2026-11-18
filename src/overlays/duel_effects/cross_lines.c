#include "../../types.h"
#include "../../psyq/stdio.h"
#include "../../game/graphics_constants.h"
#include "drawing_helpers.h"
#include "render_state_semantic_views.h"
#include "fallback_semantic_views.h"

#ifndef VERSION_EUROPE
const char gDuelEffect_ErrorMessage[] = "Error:In Effect ...\n";
const char gDuelEffect_InvalidVariantMessage[] =
    "Invalid ID. Expected ID is more small. Exit Effect Sequence.\n";
#endif

void DuelEffect_DrawErrorCross(s32 mode)
{
    GsLINE line;

    line.attribute = 0x50000000;
    line.r = line.g = line.b = 255;
    line.x0 = 0;
    line.y0 = 0;
    line.x1 = GRAPHICS_DEFAULT_WIDTH;
    line.y1 = GRAPHICS_DEFAULT_HEIGHT;
    GsSortLine(&line, gDuelEffect_pOrderingTable, 0);
    line.x0 = GRAPHICS_DEFAULT_WIDTH;
    line.y0 = 0;
    line.x1 = 0;
    line.y1 = GRAPHICS_DEFAULT_HEIGHT;
    GsSortLine(&line, gDuelEffect_pOrderingTable, 0);
#ifndef VERSION_EUROPE
    if ((u16)mode != 0) {
        printf(gDuelEffect_ErrorMessage);
        if ((u16)mode == 1) {
            printf(gDuelEffect_InvalidVariantMessage);
        }
    }
#endif
}
