#include "../../types.h"
#include "utility_helpers.h"

void func_8014FE00(SVECTOR *vertices, u16 size, u16 index)
{
    vertices[0].vx = (s16)func_8014F524(-1, index + 1) * size;
    vertices[0].vy = (index - 2 < 0 ? -1 : 1) * size;
    vertices[0].vz = 0;
    vertices[1].vx = 0;
    vertices[1].vy = (index - 2 < 0 ? -1 : 1) * size;
    vertices[1].vz = 0;
    vertices[2].vx = (s16)func_8014F524(-1, index + 1) * size;
    vertices[2].vy = 0;
    vertices[2].vz = 0;
    vertices[3].vx = 0;
    vertices[3].vy = 0;
    vertices[3].vz = 0;
}

void func_8014FED4(SVECTOR *first, SVECTOR *second)
{
    SVECTOR temporary;

    temporary = *first;
    *first = *second;
    *second = temporary;
}
