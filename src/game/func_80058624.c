#include "../types.h"
#include "../psyq/libgte.h"

typedef struct {
    s32 x0;
    s32 pad4;
    s32 x8;
    s32 xC;
    s32 pad10;
    s32 x14;
} LocalVectorSet;

extern LocalVectorSet D_800F56F0;
extern void SD_SEPlay(u16, u8, s8);
extern void func_80048A28(u16, u8, s8);

void func_80058624(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    u8 st[2];
    s8 *pp;
    u8 *p;
    s32 dx;
    s32 dz;
    s32 a;
    s32 b;
    s32 c;
    s32 d;
    s32 ex;
    s32 ez;
    s32 n;
    s32 r;
    s32 dd;
    s32 v;
    s32 w;

    dx = D_800F56F0.x0 - arg2;
    p = (u8 *)&D_800F56F0;
    dz = *(s32 *)(p + 8) - arg3;
    pp = (s8 *)&st[1];
    st[0] = 0;
    st[1] = 0;
    r = SquareRoot0(dx * dx + dz * dz);
    b = *(s32 *)(p + 8);
    c = *(s32 *)(p + 0xC);
    d = *(s32 *)(p + 0x14);
    ez = d - b;
    a = D_800F56F0.x0;
    ex = a - c;
    n = c * b - a * d;
    dd = SquareRoot0(ez * ez + ex * ex);
    r -= 0x2BC;

    st[0] = 0xFF;
    *pp = 0;
    if (r > 0) {
        v = 0xFF - r / 12;
        if (v < 0x10) {
            v = 0x10;
        }
        st[0] = v;
    }

    if (dd != 0) {
        r = (arg2 * ez + arg3 * ex + n) / dd;
        w = r / 16;
        /* The conditional form negates w's register instead of v in place. */
        v = __builtin_abs(w);
        if (v >= 0x80) {
            v = 0x7F;
        }
        w = v;
        if (r <= 0) {
            w = -w;
        }
        *pp = w;
    }

    if (arg1 != 0) {
        SD_SEPlay(arg0 & 0xFFFF, st[0], *(s8 *)&st[1]);
    } else {
        func_80048A28(arg0 & 0xFFFF, st[0], *(s8 *)&st[1]);
    }
}
