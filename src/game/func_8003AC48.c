#include "../types.h"
#include "display_effect_lifecycle.h"
#include "graphics_frame.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "func_8003A440.h"
#include "func_8003A1EC.h"
#include "func_8003AC48.h"

void func_8003AC48(u8 *p)
{
    u8 *h;
    u8 *d;
    u8 *e;
    s32 x;
    s32 y;
    s32 t;
    s32 u;
    s32 m;
    s32 i;

    if (func_80039F1C((DisplayEffectState *)p) == 0) {
        p[0x32] |= 0x10;
        h = *(u8 **)p;
        x = *(s8 *)(h + 0x16);
        y = h[0x67];
        func_8003A440((u8 **)p, (GsALON | GsAONE), x);
        d = p + 0xC;
        func_8003A1EC(p, d, y);
        func_8003A440((u8 **)d, (GsALON | GsATWO), x - 1);
        *(s16 *)(p + 0x40) = 0x80;
    }

    t = *(u16 *)(p + 0x40) - (D_8009B0D8 << 3);
    *(s16 *)(p + 0x40) = t;
    u = (s16)t;
    if (u <= 0) {
        p[0x33] = 0;
        func_80039F90((void **)(p + 0xC));
        func_80039FD4(p);
    } else {
        m = u;
        m |= (m << 8) | (m << 16);
        for (i = 2; i >= 0; i--) {
            e = ((u8 **)p)[i];
            if (e != 0) {
                *(u32 *)(e + 0xC) = m;
            }
            e = ((u8 **)p)[i + 3];
            if (e != 0) {
                *(u32 *)(e + 0xC) = m;
            }
        }
    }
}
