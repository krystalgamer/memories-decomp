#include "../types.h"
#include "../psyq/libgte.h"

extern s32 func_80039F1C(u8 *);
extern void func_8003A95C(u8 *, s32, s32);

void func_8003A990(u8 *p)
{
    s32 d;
    s32 t;
    s32 c;
    s32 dx;
    s32 dy;

    if (func_80039F1C(p) == 0) {
        *(s16 *)(p + 0x48) = ONE / 4;
        d = (ONE / 4) / *(s16 *)(p + 0x44);
        *(s16 *)(p + 0x4A) = d;
        if (d >= 0) {
            *(s16 *)(p + 0x48) = 0;
        }
        *(s16 *)(p + 0x44) = *(u16 *)(p + 0x40) - *(u16 *)(p + 0x34);
        *(s16 *)(p + 0x46) = *(u16 *)(p + 0x42) - *(u16 *)(p + 0x36);
    }

    t = *(u16 *)(p + 0x48) + *(u16 *)(p + 0x4A);
    *(s16 *)(p + 0x48) = t;

    if ((u16)(t - 1) >= (ONE / 4) - 1) {
        func_8003A95C(p, *(s16 *)(p + 0x40), *(s16 *)(p + 0x42));
        p[0x33] = 0;
    } else {
        c = rcos((s16)t);
        dx = c * *(s16 *)(p + 0x44) / ONE;
        dy = c * *(s16 *)(p + 0x46) / ONE;
        if (*(s16 *)(p + 0x4A) < 0) {
            dx = *(s16 *)(p + 0x44) - dx;
            dy = *(s16 *)(p + 0x46) - dy;
        }
        func_8003A95C(
            p,
            (s16)(*(u16 *)(p + 0x40) - dx),
            (s16)(*(u16 *)(p + 0x42) - dy)
        );
    }
}
