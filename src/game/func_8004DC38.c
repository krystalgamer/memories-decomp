#include "../types.h"

void func_8004DC38(u8 *p, s32 i, s32 n, s32 amount)
{
    u8 *e;
    u8 *q;
    u8 *cnt;
    u8 *w;
    s32 off;
    s32 k;
    u32 total;
    s32 rem;
    s32 count;
    s32 j;
    s32 v;

    off = i * 4;
    e = *(u8 **)(p + off + 0x1E0);
    k = *(u16 *)(p + (n * 0x74 + i * 2) + 0x2C8);
    q = *(u8 **)(p + 0xDD8) + k * 4;
    if (e == (u8 *)0) {
        return;
    }
    if (k == 0xFFFF) {
        return;
    }
    if (*(u16 *)(p + (n * 0x76 + i * 2) + 0x750) == 0) {
        return;
    }
    total = *(u16 *)(p + (n * 0x76 + i * 2) + 0x750) * 16;
    rem = amount % total;
    cnt = q - 4;
    if (rem == 0 && amount != 0 && p[0xE16] != 0x3C) {
        rem = total;
    }
    j = 0;
    if (j >= *(u16 *)cnt) {
        return;
    }
    for (; j < *(u16 *)cnt; j++) {
        if (p[0xE16] != 0x3C) {
            if (rem - (q[2] << 4) <= 0) {
                break;
            }
        } else {
            if (rem - (q[2] << 4) < 0) {
                break;
            }
        }
        rem -= q[2] << 4;
        if (*(s32 *)(q + 4) < 0) {
            break;
        }
        q += 4;
    }
    count = *(u16 *)cnt;
    w = q;
    if (j >= count) {
        return;
    }
    for (; j < count; j++) {
        if (*(s32 *)(w + 4) < 0) {
            break;
        }
        w += 4;
    }
    v = (q - *(u8 **)(p + 0xDD8)) >> 2;
    *(s16 *)(e + 0x16) = v;
    if (v != 0) {
        *(s16 *)(e + 0x14) = v - 1;
    } else {
        *(s16 *)(e + 0x14) = (w - *(u8 **)(p + 0xDD8)) >> 2;
    }
    v = q[2] << 4;
    *(s16 *)(e + 0x12) = v;
    *(s16 *)(e + 0x10) = v - rem;
    if (p[0xE16] == 0x3C && *(s16 *)(e + 0x10) >= *(u16 *)(e + 0x12)) {
        *(s16 *)(e + 0x10) = 0x6000;
    }
}
