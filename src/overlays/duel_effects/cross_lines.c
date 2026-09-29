#include "../../types.h"
#include "../../psyq/stdio.h"
#include "../../game/graphics_constants.h"
#include "drawing_helpers.h"

void func_8014E35C(s32 mode)
{
    GsLINE line;

    line.attribute = 0x50000000;
    line.r = line.g = line.b = 255;
    line.x0 = 0;
    line.y0 = 0;
    line.x1 = GRAPHICS_DEFAULT_WIDTH;
    line.y1 = GRAPHICS_DEFAULT_HEIGHT;
    GsSortLine(&line, D_8015B7F4, 0);
    line.x0 = GRAPHICS_DEFAULT_WIDTH;
    line.y0 = 0;
    line.x1 = 0;
    line.y1 = GRAPHICS_DEFAULT_HEIGHT;
    GsSortLine(&line, D_8015B7F4, 0);
#ifndef VERSION_EUROPE
    if ((u16)mode != 0) {
        printf(gNorthAmerican_D_80146014);
        if ((u16)mode == 1) {
            printf(gNorthAmerican_D_8014602C);
        }
    }
#endif
}
