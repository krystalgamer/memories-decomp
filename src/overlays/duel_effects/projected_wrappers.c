#include "../../types.h"
#include "../../game/gpu_packets.h"
#include "packet_helpers.h"
#include "textured_quads.h"

void func_80151218(POLY_FT4 *packet, SVECTOR *vertices, s16 bias, u16 mode)
{
    s32 p;
    s32 flag;
    s32 depth;

    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1,
                        (PSXLONG *)&packet->x2, (PSXLONG *)&packet->x3,
                        (PSXLONG *)&p, (PSXLONG *)&flag);
    if (flag >= 0) {
        if (bias == 0) {
            func_80152F9C(packet, mode);
        } else {
            depth = depth - (bias - 1);
            if (depth < 0) {
                depth = 0;
            }
            if (mode == 0) {
                packet->code &= ~2;
                GsSortPoly(packet, D_8015B7F4, (u16)(depth >> 2));
            } else {
                func_8005B260((u32 *)packet, D_8015B7F4, (u16)(depth >> 2), mode);
            }
        }
    }
}

void func_8015131C(POLY_GT4 *packet, SVECTOR *vertices, s16 bias, u16 mode)
{
    s32 p;
    s32 flag;
    s32 depth;

    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1,
                        (PSXLONG *)&packet->x2, (PSXLONG *)&packet->x3,
                        (PSXLONG *)&p, (PSXLONG *)&flag);
    if (flag >= 0) {
        if (bias == 0) {
            func_801530B0(packet, mode);
        } else {
            depth = depth - (bias - 1);
            if (depth < 0) {
                depth = 0;
            }
            func_8005B260((u32 *)packet, D_8015B7F4, (u16)(depth >> 2), mode);
        }
    }
}
