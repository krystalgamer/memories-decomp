#include "../../types.h"
#include "utility_helpers.h"

void func_8014EA7C(u16 radius, SVECTOR *vertices)
{
    s32 i;
    s32 angle;

    for (i = 0; i < 32; i++) {
        angle = i << 7;
        vertices[i].vx = radius * ccos(angle) / 4096;
        vertices[i].vy = radius * csin(angle) / 4096;
        vertices[i].vz = 0;
    }
}
