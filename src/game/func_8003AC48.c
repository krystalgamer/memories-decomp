#include "../types.h"

extern s32 D_8009B0D8;

extern s32 func_80039F1C(u8 *);
extern void func_8003A440(u8 **, u32, s32);
extern s32 func_8003A1EC(u8 *, u8 *, s32);
extern void func_80039F90(void *);
extern void func_80039FD4(u8 *);

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

    if (func_80039F1C(p) == 0) {
        p[0x32] |= 0x10;
        h = *(u8 **)p;
        x = *(s8 *)(h + 0x16);
        y = h[0x67];
        func_8003A440((u8 **)p, 0x50000000, x);
        d = p + 0xC;
        func_8003A1EC(p, d, y);
        func_8003A440((u8 **)d, 0x60000000, x - 1);
        *(s16 *)(p + 0x40) = 0x80;
    }

    t = *(u16 *)(p + 0x40) - (D_8009B0D8 << 3);
    *(s16 *)(p + 0x40) = t;
    u = (s16)t;
    if (u <= 0) {
        p[0x33] = 0;
        func_80039F90(p + 0xC);
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
