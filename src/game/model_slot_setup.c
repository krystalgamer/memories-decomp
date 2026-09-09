#include "../types.h"
#include "model.h"

/* One model slot's setup: the reset that gives it its defaults (0x8005611C)
   and the per-frame duel-side layout pass that reads them (0x80056250). The
   two are contiguous -- 0x8005611C is 0x134 bytes and ends exactly at
   0x80056250 -- and are bounded above by model_load_monster_merge.c at a
   different profile.

   They are one initializer and its consumer. The reset writes the mode byte
   at +0xE16 as 0x3E, the pair at +0xE0C/+0xE0D as 7 and 8, the halfword at
   +0xE0A as 0x1000 and the busy byte at +0xE1F as 0; the layout pass switches
   on that same +0xE16 (0x3E is one of its three cases), reads +0xE0C, +0xE0D
   and +0xE0A back, and sets +0xE1F to 1. */

void func_8005B5FC(s32 *destination, s32 value, u32 count);
void Model_InitLightTriplet(s32 index);
void func_8004CB0C(void);
void func_8004D75C(s32 arg0);
void func_8004D914(s32 arg0);
void func_800582C0(s32 arg0, s32 arg1, s32 arg2);
void func_800590DC(s32 arg0);
void func_8005A468(s32 arg0, s32 arg1);
void func_8005A4C4(u8 *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);

void func_8005611C(s32 arg0)
{
    u8 *p;
    u8 *q;
    s32 i;
    s32 n;

    p = (u8 *)D_800F2C40 + arg0 * MODEL_SLOT_SIZE;
    func_8005B5FC((s32 *)p, 0, 0x388);

    *(s32 *)(p + 0xDA8) = MODEL_FIXED_HALF;
    *(s32 *)(p + 0xDA4) = MODEL_FIXED_HALF;
    *(s32 *)(p + 0xDA0) = MODEL_FIXED_HALF;
    *(s32 *)(p + 0xDB8) = MODEL_FIXED_ONE;
    *(s32 *)(p + 0xDB4) = MODEL_FIXED_ONE;
    *(s32 *)(p + 0xDB0) = MODEL_FIXED_ONE;
    p[0xDC2] = 0x80;
    p[0xDC1] = 0x80;
    p[0xDC0] = 0x80;
    p[0xE0C] = 7;
    p[0xE0D] = 8;
    p[0xE14] = 0xFF;
    p[0xDC3] = 0;
    *(s16 *)(p + 0xE0A) = 0x1000;
    p[0xE1D] = 0;
    p[0xDFE] = arg0;
    p[0xDFF] = 0;
    *(u16 *)(p + 0xDFC) = 0xFFFF;
    *(u16 *)(p + 0xDFA) = 0xFFFF;

    n = 1;
    i = 3;
    q = p + i;
    for (; i >= 0; q--, i--) {
        q[0xBF4] = n;
    }

    for (i = 0; i < 0x40; i++) {
        *(s32 *)(p + i * 4 + 0xBF8) &= 0x8000FFFF;
        p[i * 4 + 0xBF9] = 0;
        p[i * 4 + 0xBF8] = 0;
    }

    *(s32 *)(p + 0xD08) = -1;
    *(s32 *)(p + 0xD0C) = -1;
    *(s32 *)(p + 0xD10) = -1;
    p[0xE16] = 0x3E;
    p[0xE1F] = 0;
    Model_InitLightTriplet(arg0);
}

/* Duel-side layout pass for one player's model slot: sums the hand's card
 * widths (0x14 for the cards flagged in the +0xBEC bitfield, 0xC otherwise),
 * derives the two cursor limits at +0xDF0/+0xDF4, resets each card object's
 * sprite fields, and applies the mode-dependent horizontal offset through
 * func_8005A468 before func_800582C0 draws it. */
void func_80056250(s32 arg0, u8 *arg1, s32 arg2, s32 arg3) {
    u8 *p;
    u8 *q;
    u8 **c;
    u8 *e;
    u8 *r;
    s32 i;
    s32 j;
    s32 sum;
    s32 k;
    s32 v;
    s32 m;
    s32 n;
    u8 *b;
    u8 *b2;
    s32 w;

    func_8004CB0C();
    if (arg1 == (u8 *)0) {
        return;
    }
    func_8004D75C(arg0);
    func_8004D914(arg0);
    p = arg0 * MODEL_SLOT_SIZE + (u8 *)D_800F2C40;
    sum = 0;
    if (arg0 < 2) {
        i = sum;
        if (sum < p[0xE1B]) {
            m = 0xFFFF;
            n = p[0xE1B];
            do {
                if (*(u16 *)(p + i * 2 + 0x33C) != m) {
                    k = i / 8;
                    if (((p + k)[0xBEC] >> (i - k * 8)) & 1) {
                        sum += 0x14;
                    } else {
                        sum += 0xC;
                    }
                }
                i++;
            } while (i < n);
        }
    }
    v = (*(s32 *)(p + 0xDF0) = *(s32 *)(p + 0xDE0) + sum);
    *(s32 *)(p + 0xDF4) = v + *(u16 *)(p + 0xE02) * 4;
    func_8005A4C4(p, 0, 0, 0, arg0 == 1 ? 0x800 : 0);

    q = arg0 * MODEL_SLOT_SIZE + (u8 *)D_800F2C40;
    c = (u8 **)(q + 0x1E0);
    for (j = 0; j < q[0xE1B]; j++) {
        *(u16 *)(*c + 8) = 0xFFFF;
        *(u16 *)(*c + 0x16) = *(u16 *)(*c + 0x18);
        *(u16 *)(*c + 0xA) = 0xFFFF;
        (*c)[0xC] = (*c)[0x1A];
        *(u16 *)(*c + 0x10) = 0;
        (*c)[0xD] = 0x10;
        c++;
    }

    b = (u8 *)D_800F2C40;
    e = arg0 * MODEL_SLOT_SIZE + b;
    e[0xE1F] = 1;
    func_800590DC(arg0);
    v = e[0xE0D] * 2;
    w = v;
    switch (e[0xE16]) {
    case 0x23:
        w = 0;
        func_8005A468(arg0, w);
        break;
    case 0x3E:
        w = v;
        func_8005A468(arg0, w);
        break;
    case 0x3C:
        w = -w;
        func_8005A468(arg0, w);
        break;
    }
    if (arg0 < 2) {
        b2 = (u8 *)D_800F2C40;
        r = arg0 * MODEL_SLOT_SIZE + b2;
        func_800582C0(arg0, r[0xE0C], *(u16 *)(r + 0xE0A));
    }
}
