#include "../../types.h"
#include "../../game/gpu_packets.h"
#include "layered_drawing.h"

void func_801558F4(u8 *color, SVECTOR *inner, SVECTOR *outer, u16 bias, u16 flags)
{
    POLY_G4 gradient;
    POLY_G4 *edge = &gradient;
    POLY_FT4 polygon;
    POLY_FT4 *packet = &polygon;
    SVECTOR vertices[4];
    s32 p;
    s32 flag;
    s32 depth;
    s32 i;
    s32 j;

    setPolyG4(edge);
    setRGB0(edge, 0, 0, 0);
    setRGB1(edge, 0, 0, 0);
    setRGB2(edge, color[0] >> 1, color[1] >> 1, color[2] >> 1);
    setRGB3(&gradient, color[0] >> 1, color[1] >> 1, color[2] >> 1);
    setSemiTrans(edge, 1);
    setPolyFT4(packet);
    packet->tpage = D_8015B748.named.page_at_00;
    packet->clut = D_8015B748.named.clut_at_02;
    setUV4(packet, 0, 0, 63, 0, 0, 63, 63, 63);
    setRGB0(packet, color[0], color[1], color[2]);
    setSemiTrans(packet, 1);
    for (i = 0; i < 4; i++) {
        depth = RotAverage4(&outer[i], &outer[(i + 1) % 4],
            &inner[i], &inner[(i + 1) % 4],
            (PSXLONG *)&edge->x0, (PSXLONG *)&edge->x1,
            (PSXLONG *)&edge->x2, (PSXLONG *)&edge->x3,
            (PSXLONG *)&p, (PSXLONG *)&flag);
        if (flag >= 0) {
            depth++;
            if (bias == 0) {
                func_80152EC4(edge, flags);
            } else {
                depth -= bias;
                func_8005B260((u32 *)edge, D_8015B7F4,
                    (u16)(depth >> 2), flags);
            }
        }
        if (bias == 0) {
            func_8014F358(vertices, 8);
        } else {
            func_8014F358(vertices, 16);
        }
        for (j = 0; j < 4; j++) {
            addVector(&vertices[j], &inner[i]);
        }
        func_80151218(packet, vertices, (s16)bias, flags);
    }
}
