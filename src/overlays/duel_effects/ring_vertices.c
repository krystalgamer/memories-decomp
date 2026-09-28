#include "../../types.h"
#include "utility_helpers.h"

void func_8014EA7C(u16 radius, SVECTOR *vertices)
{
    s32 i;

    for (i = 0; i < 32; i++) {
        vertices[i].vx = radius * ccos(i * 128) / 4096;
        vertices[i].vy = radius * csin(i * 128) / 4096;
        vertices[i].vz = 0;
    }
}

void func_8014EB1C(u16 width, u16 height, SVECTOR *vertices, u16 count)
{
    s32 i;
    s32 angle;

    width = width * (csin(512) << 1) / 4096;
    height = height * (csin(512) << 1) / 4096;
    for (i = 0; i < count; i++) {
        angle = (4096 / count) * i + (u32)(4096 / count) / 2;
        vertices->vx = width * ccos(angle) / 4096;
        vertices->vy = height * csin(angle) / 4096;
        vertices->vz = 0;
        vertices++;
    }
}
