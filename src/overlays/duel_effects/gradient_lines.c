#include "../../types.h"
#include "drawing_helpers.h"

u16 func_801570B0(u8 *color, u16 count, SVECTOR *vertices, u16 bias,
                    u16 mode, SVECTOR *output, u16 flags, SVECTOR *offset)
{
    GsGLINE line;
    GsGLINE *packet = &line;
    s32 p;
    s32 flag;
    s32 depth;
    s32 divisor;
    s32 i;

    packet->attribute = 0x40000000U + ((u32)flags << 28);
    for (i = 0; i < count; i++) {
        divisor = count + 1;
        setRGB0(packet, color[0] / (divisor - i),
            color[1] / (divisor - i), color[2] / (divisor - i));
        setRGB1(packet, color[0] / (count - i),
            color[1] / (count - i), color[2] / (count - i));
        RotTransPers(&vertices[i], (PSXLONG *)&packet->x0, (PSXLONG *)&p, (PSXLONG *)&flag);
        depth = RotTransPers(&vertices[i + 1], (PSXLONG *)&packet->x1,
                            (PSXLONG *)&p, (PSXLONG *)&flag);
        depth -= bias;
        if (depth >= 0) {
            GsSortGLine(packet, D_8015B7F4, (u16)(depth >> 2));
        }
        if (mode == 1 && vertices[i + 1].vy > 0) {
            setVector(output, vertices[i + 1].vx, 0, vertices[i + 1].vz);
            addVector(output, offset);
            mode = 2;
        }
    }
    return mode;
}
