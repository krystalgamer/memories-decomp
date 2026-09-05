#include "../types.h"

extern u8 *D_8009B458;
extern s32 D_80011434[];

s32 func_8004A3BC(u8 *arg0, s32 arg1);
s32 func_80049FB4(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void func_80077450(u8 *arg0);

/* Re-derives one record's voice parameters when its bank byte changed (or
 * when forced): stores the new bank in the record, converts the pitch through
 * func_8004A3BC into a 7.7 fixed value, fills the request block at
 * D_8009B458 + 0x4C0 and submits it through func_80077450. */
void func_8004A43C(u8 *p, s32 force) {
    u8 *e;
    s32 v;
    s32 x;
    s32 y;
    u8 *b;

    e = D_8009B458 + p[3] * 24;
    if (e[7] == *(s16 *)(p + 0x1A) && force == 0) {
        return;
    }
    *(s16 *)(p + 0x1A) = e[7];
    v = func_8004A3BC(p, e[7]);
    v = v + (p[6] << 7);
    x = (s16)v >> 7;
    y = v & 0x7F;
    b = D_8009B458;
    *(s32 *)(b + 0x4C4) = 0x10;
    *(s32 *)(b + 0x4C0) = D_80011434[p[0]];
    v = func_80049FB4(x, y, p[0x12], p[0x13]);
    *(s16 *)(D_8009B458 + 0x4D4) = v;
    func_80077450(D_8009B458 + 0x4C0);
}
