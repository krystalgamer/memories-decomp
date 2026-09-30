#include "../../types.h"
#include "../../game/gpu_packets.h"
#include "drawing_helpers.h"

void func_80156E58(u8 *color, u16 width, SVECTOR *positions, u16 count, s16 bias)
{
    SVECTOR left;
    SVECTOR right;
    POLY_G3 polygon;
    POLY_G3 *packet = &polygon;
    s32 p;
    s32 flag;
    s32 i;
    s32 depth;

    setPolyG3(packet);
    setRGB0(packet, 0, 0, 0);
    setRGB1(packet, color[0], color[1], color[2]);
    setRGB2(packet, color[0], color[1], color[2]);
    left.vx = -width;
    left.vy = 0;
    left.vz = 0;
    right.vx = width;
    right.vy = 0;
    right.vz = 0;
    for (i = 0; i < count; i++) {
        depth = RotAverage3(&positions[i], &left, &right,
            (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
            (PSXLONG *)&p, (PSXLONG *)&flag) - bias;
        func_8005B260((u32 *)packet, D_8015B7F4, (u16)(depth >> 2), 1);
    }
}

void func_80156FA4(u8 *color, SVECTOR *vertices, u16 flags, u16 mode)
{
    POLY_F4 polygon;
    POLY_F4 *packet = &polygon;
    s32 p;
    s32 flag;
    s32 depth;

    setPolyF4(packet);
    setRGB0(packet, color[0], color[1], color[2]);
    setSemiTrans(packet, 1);
    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
        (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
        (PSXLONG *)&packet->x3, (PSXLONG *)&p, (PSXLONG *)&flag);
    if (flag >= 0) {
        if (mode == 0) {
            setSemiTrans(packet, 0);
            GsSortPoly(packet, D_8015B7F4, depth >> 2);
        } else {
            func_8005B260((u32 *)packet, D_8015B7F4, (u16)(depth >> 2), flags);
        }
    }
}
