#include "../../types.h"
#include "utility_helpers.h"

void func_8014EC8C(u16 width, u16 depth, s32 level, s32 offset,
                   SVECTOR *vertices, u16 count)
{
    s32 i;
    s32 angle;
    s32 height;

    width = width * (csin(512) << 1) / 4096;
    depth = depth * (csin(512) << 1) / 4096;
    for (i = 0; i < count; i++) {
        angle = (4096 / count) * i + (u32)(4096 / count) / 2;
        height = level - offset;
        vertices->vx = width * ccos(angle) / 4096;
        vertices->vy = height;
        vertices->vz = depth * csin(angle) / 4096;
        vertices++;
    }
}
