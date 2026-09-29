#include "../../types.h"
#include "utility_helpers.h"

void func_8014EC8C(u16 width, u16 depth, s32 top, s32 bottom,
                   SVECTOR *vertices, u16 count)
{
    s32 i;
    s32 angle;
    s32 length;

    width = width * (csin(512) << 1) / 4096;
    depth = depth * (csin(512) << 1) / 4096;
    i = 0;
    if (count != 0) {
        length = count;
        bottom = top - bottom;
        do {
            angle = (4096 / length) * i + (u32)(4096 / length) / 2;
            vertices->vx = width * ccos(angle) / 4096;
            vertices->vy = bottom;
            vertices->vz = depth * csin(angle) / 4096;
            vertices++;
            i++;
        } while (i < length);
    }
}
