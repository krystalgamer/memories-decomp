#include "../../types.h"
#include "utility_helpers.h"

void func_8014F490(SVECTOR *vertices, s32 width, s16 height)
{
    s32 i;
    s32 horizontal = width;

    for (i = 0; i < 4; i++) {
        vertices[i].vx = horizontal * func_8014F524(-1, i + 1);
        vertices[i].vy = (i - 2 < 0 ? -1 : 1) * height;
        vertices[i].vz = 0;
    }
}
