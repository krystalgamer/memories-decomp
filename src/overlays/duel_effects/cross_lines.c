#include "../../types.h"
#include "drawing_helpers.h"

void func_8014E35C(s32 mode)
{
    GsLINE line;

    line.attribute = 0x50000000;
    line.r = line.g = line.b = 255;
    line.x0 = 0;
    line.y0 = 0;
    line.x1 = 320;
    line.y1 = 256;
    GsSortLine(&line, D_8015B7F4, 0);
    line.x0 = 320;
    line.y0 = 0;
    line.x1 = 0;
    line.y1 = 256;
    GsSortLine(&line, D_8015B7F4, 0);
}
