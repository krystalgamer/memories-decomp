#include "../../types.h"
#include "packet_helpers.h"
#include "drawing_helpers.h"
#include "textured_quads.h"

void func_801566D4(s32 value, u8 *color, SVECTOR *offset, u16 mode, u16 size, u16 bias)
{
    SVECTOR vertices[20];
    u16 digits[8];
    POLY_FT4 polygon;
    POLY_FT4 copy;
    POLY_FT4 *packet = &polygon;
    s32 p;
    s32 flag;
    s32 depth;
    s32 adjusted;
    s32 i;
    u16 count;
    u16 vertex;
    u16 draw_mode;
    POLY_FT4 *draw_packet;
    u16 tile;

    count = func_80156AD4(value);
    func_80156B40(__builtin_abs(value), digits);
    vertex = 0;
    setPolyFT4(packet);
    setRGB0(packet, color[0], color[1], color[2]);
    for (i = 0; i < count * 2 + 4; i++) {
        setVector(&vertices[i],
            size * (i / 2 << 1)
                - ((count + 1) / 2 * size * 2 + size * ((count + 1) % 2)),
            (s16)func_8014F524(-1, i + 1) * size, 0);
        addVector(&vertices[i], offset);
    }
    for (i = 0; i < count + 1; i++) {
        packet->tpage = D_8015B748.pairs[9][0];
        packet->clut = D_8015B748.pairs[9][1];
        depth = RotAverage4(&vertices[vertex], &vertices[vertex + 2],
            &vertices[vertex + 1], &vertices[vertex + 3],
            (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1,
            (PSXLONG *)&packet->x2, (PSXLONG *)&packet->x3,
            (PSXLONG *)&p, (PSXLONG *)&flag);
        if (i == 0) {
            if (value > 0) {
                tile = 10;
            } else {
                tile = 11;
            }
        } else {
            tile = digits[i - 1];
        }
        setUV4(packet,
            D_8015B3C0[tile].x, D_8015B3C0[tile].y,
            D_8015B3C0[tile].x + D_8015B3C0[tile].w, D_8015B3C0[tile].y,
            D_8015B3C0[tile].x, D_8015B3C0[tile].y + D_8015B3C0[tile].h,
            D_8015B3C0[tile].x + D_8015B3C0[tile].w,
            D_8015B3C0[tile].y + D_8015B3C0[tile].h);
        copy = polygon;
        if (bias == 0) {
            if (flag >= 0) {
                if (mode == 1) {
                    draw_packet = packet;
                    draw_mode = 1;
                } else if (mode == 0) {
                    draw_packet = packet;
                    draw_mode = 0;
                } else {
                    vertex += 2;
                    continue;
                }
                func_80152F9C(draw_packet, draw_mode);
            }
        } else {
            adjusted = depth + 1;
            depth = adjusted - bias;
            if (depth >= 0 && flag >= 0) {
                if (mode == 1) {
                    func_8005B260((u32 *)packet, D_8015B7F4, (u16)(depth >> 2), 1);
                } else {
                    func_8005B260((u32 *)packet, D_8015B7F4, (u16)(depth >> 2), 0);
                }
            }
        }
        vertex += 2;
    }
}
