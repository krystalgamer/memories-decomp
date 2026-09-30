#include "../../types.h"
#include "../../game/gpu_packets.h"
#include "drawing_helpers.h"

void func_80156448(u8 *color, SVECTOR *first, SVECTOR *middle, SVECTOR *last, u16 bias)
{
    POLY_G4 polygon;
    POLY_G4 *packet = &polygon;
    s32 p;
    s32 flag;
    s32 depth;
    s32 i;

    setPolyG4(packet);
    setRGB0(packet, 0, 0, 0);
    setRGB1(packet, 0, 0, 0);
    setRGB2(packet, color[0] >> 1, color[1] >> 1, color[2] >> 1);
    setRGB3(&polygon, color[0] >> 1, color[1] >> 1, color[2] >> 1);
    for (i = 0; i < 32; i++) {
        depth = RotAverage4(&first[i], &first[(i + 1) % 32],
            &middle[i], &middle[(i + 1) % 32],
            (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1,
            (PSXLONG *)&packet->x2, (PSXLONG *)&packet->x3,
            (PSXLONG *)&p, (PSXLONG *)&flag);
        if (flag >= 0) {
            depth -= bias;
            if (bias == 0) {
                func_80152EC4(packet, 1);
            } else {
                if (depth < 0) {
                    depth = 0;
                }
                func_8005B260((u32 *)packet, D_8015B7F4, (u16)(depth >> 2), 1);
            }
        }
        depth = RotAverage4(&last[i], &last[(i + 1) % 32],
            &middle[i], &middle[(i + 1) % 32],
            (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1,
            (PSXLONG *)&packet->x2, (PSXLONG *)&packet->x3,
            (PSXLONG *)&p, (PSXLONG *)&flag);
        if (flag >= 0) {
            depth -= bias;
            if (bias == 0) {
                func_80152EC4(packet, 1);
            } else {
                if (depth < 0) {
                    depth = 0;
                }
                func_8005B260((u32 *)packet, D_8015B7F4, (u16)(depth >> 2), 1);
            }
        }
    }
}
