#include "../../types.h"
#include "apv2.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"

extern u8 D_80169494[][2];

void func_80169284(void)
{
    Apv2FramePoly poly;
    s32 i;
    s32 j;
    s32 k;
    s32 idx;
    s32 n;
    s8 *d;

    poly.tag = 0x05000000;
    poly.code = 0x280000FF;
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 4; j++) {
            Apv2FramePoly *pp = &poly;
            s8 dir[8] = { 1, 1, -1, 1, 1, -1, -1, -1 };

            for (k = 0; k < 4; k++) {
                d = &dir[j * 2];
                n = (k + 2) >> 2;
                idx = i + (k & 1);
                pp->xy[k] = ((d[1] * D_80169494[16 - idx][n] + 120) << 16)
                           | (d[0] * D_80169494[idx][n] + 160);
            }
            DrawPrim(&poly);
        }
    }
    poly.xy[0] = 0x00500070;
    poly.xy[1] = 0x00480078;
    poly.xy[2] = 0x00A800C8;
    poly.xy[3] = 0x00A000D0;
    DrawPrim(&poly);
}
