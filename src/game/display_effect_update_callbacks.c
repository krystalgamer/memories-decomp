/* Two display-effect update callbacks, on the same DisplayEffectState record
 * and the same first-frame latch func_80039F1C.
 *
 * The two disagree about how wide the per-frame step multiplier is, and
 * graphics_frame.h says a unit that needs two spellings takes an alias rather
 * than a second declaration -- display_object_fade_callbacks.c already does
 * this for the plain and volatile pair. Here the unit keeps the plain s32 arm
 * for func_8003AD6C and func_8003B054 reaches the same symbol as a halfword,
 * which is the read its retail code does. */
#include "../types.h"
#include "display_object_config.h"
#include "display_effect_lifecycle.h"
#include "graphics_frame.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "func_8003A440.h"
#include "func_8003A1EC.h"
#include "display_object_api.h"
#include "display_object_helpers.h"

extern u16 D_8009B0D8_halfword asm("D_8009B0D8");

extern void func_8003A920(u8 *, s16, s16);

void func_8003AD6C(u8 *p)
{
    s32 dd[2];
    u8 *r;
    s32 a;
    s32 b;
    s32 d;
    s32 m;
    s16 n;
    s32 c;
    s32 w;
    s16 y;
    s32 x;
    u8 *o;

    if (func_80039F1C((DisplayEffectState *)p) == 0) {
        p[0x32] |= 0x10;
        r = *(u8 **)p;
        a = r[0x67];
        b = *(s8 *)(r + 0x16);
        func_80039F90((void **)p);
        func_8003A1EC(p, (u8 **)p, p[0x31]);
        func_8003A440((u8 **)p, (GsALON | GsAONE), b);
        func_8003A1EC(p, (u8 **)(p + 0xC), p[0x31]);
        d = b - 1;
        func_8003A440((u8 **)(p + 0xC), (GsALON | GsATWO), d);
        func_8003A1EC(p, (u8 **)(p + 0x18), a);
        func_8003A440((u8 **)(p + 0x18), (GsALON | GsAONE), b);
        func_8003A1EC(p, (u8 **)(p + 0x24), a);
        func_8003A440((u8 **)(p + 0x24), (GsALON | GsATWO), d);
        *(s16 *)(p + 0x40) = 0x80;
    }
    n = *(u16 *)(p + 0x40) - D_8009B0D8 * 8;
    *(u16 *)(p + 0x40) = n;
    if (n <= 0) {
        p[0x33] = 0;
        func_8003A440((u8 **)p, 0, *(s8 *)(*(u8 **)p + 0x16));
        func_8003A920(p, *(s16 *)(p + 0x34), *(s16 *)(p + 0x36));
        func_80039F90((void **)(p + 0xC));
        func_80039F90((void **)(p + 0x18));
        func_80039F90((void **)(p + 0x24));
        p[0x32] &= 0xEF;
        return;
    }
    c = n;
    c |= (c << 8) | (c << 0x10);
    for (m = 2; m >= 0; m--) {
        o = *(u8 **)(p + m * 4 + 0x18);
        if (o != (u8 *)0) {
            *(s32 *)(o + 0xC) = c;
        }
        o = *(u8 **)(p + m * 4 + 0x24);
        if (o != (u8 *)0) {
            *(s32 *)(o + 0xC) = c;
        }
    }
    c = 0x80 - *(s16 *)(p + 0x40);
    c |= (c << 8) | (c << 0x10);
    for (m = 2; m >= 0; m--) {
        o = *(u8 **)(p + m * 4);
        if (o != (u8 *)0) {
            *(s32 *)(o + 0xC) = c;
        }
        o = *(u8 **)(p + m * 4 + 0xC);
        if (o != (u8 *)0) {
            *(s32 *)(o + 0xC) = c;
        }
    }
    dd[0] = *(s16 *)(p + 0x40) / 8;
    w = (0x80 - *(s16 *)(p + 0x40)) / 8;
    dd[1] = -w;
    if (p[0x3C] == 0) {
        dd[0] = -dd[0];
        dd[1] = w;
    }
    y = *(s16 *)(p + 0x36);
    x = *(s16 *)(p + 0x34) + dd[0];
    x = (s16)x;
    func_8003A920(p, x, y);
    func_8003A920(p + 0xC, x, y);
    x = *(s16 *)(p + 0x34) - dd[1];
    x = (s16)x;
    func_8003A920(p + 0x18, x, y);
    func_8003A920(p + 0x24, x, y);
}

void func_8003B054(u8 *p)
{
    u8 *o;
    u8 *o2;
    s32 id;
    s32 idx;
    s32 c;
    s32 q;

    if (func_80039F1C((DisplayEffectState *)p) == 0) {
        if (*(s16 *)(p + 0x40) != 0) {
            o = *(u8 **)p;
            *(u16 *)(o + 0x60) = 0;
        } else {
            id = *(s8 *)(p + 0x30);
            idx = id - 0x41;
            o = func_800400AC(func_8004002C(), 1);
            func_80040510((DisplayObjectConfigView *)o, *(s16 *)(p + 0x34), *(s16 *)(p + 0x36), 0x30, 0x30, 0, 0, 0xE, 0x380, 0xF0);
            *(u16 *)(o + 0x40) += (idx >> 4) << 6;
            o[0x5C] = (idx % 5) * 48;
            o[0x5D] = (idx / 5) * 48;
            *(u16 *)(o + 0x42) += idx % 16;
            *(u16 *)(o + 0x46) = 0;
            *(u32 *)(o + 0xC) = 0;
            *(u32 *)(o + 4) |= 0x51000000;
            func_80042918(o);
            func_800428EC(o, -8);
            *(u8 **)p = o;
            *(u16 *)(o + 0x60) = 0x14;
        }
        *(u32 *)(o + 4) = (*(u32 *)(o + 4) | (GsALON | GsAONE)) & ~GsROTOFF;
        *(u16 *)(o + 0x4A) = 0;
        o2 = func_800400AC(func_8004002C(), 1);
        func_80040510((DisplayObjectConfigView *)o2, *(s16 *)(o + 0x30), *(s16 *)(o + 0x32), 0x30, 0x30, o[0x5C], o[0x5D], 0xE, 0x200, 0xFD);
        *(u32 *)(o2 + 4) = (*(u32 *)(o2 + 4) | 0x61000000) & ~GsROTOFF;
        *(u16 *)(o2 + 0x46) = *(u16 *)(o + 0x46);
        *(u16 *)(o2 + 0x4A) = 0;
        func_80042918(o2);
        func_800428EC(o2, -9);
        *(u8 **)(p + 4) = o2;
    }
    o = *(u8 **)p;
    o2 = *(u8 **)(p + 4);
    if (*(s16 *)(p + 0x40) != 0) {
        *(u16 *)(o + 0x60) += D_8009B0D8_halfword;
    } else {
        *(u16 *)(o + 0x60) -= D_8009B0D8_halfword;
    }
    if (*(s16 *)(o + 0x60) <= 0) {
        *(u32 *)(o + 4) = (*(u32 *)(o + 4) & ~(GsALON | GsATWO | GsAONE)) | GsROTOFF;
        *(u32 *)(o + 0xC) = 0x808080;
        *(u16 *)(o + 0x46) = 0x1000;
        func_8004036C(o2);
        *(u8 **)(p + 4) = 0;
        goto clear;
    }
    if (*(s16 *)(o + 0x60) >= 0x14) {
        func_80039FD4(p);
    clear:
        p[0x33] = 0;
        return;
    }
    c = -0x80 - *(s16 *)(o + 0x60) * 6;
    o2[0xE] = c;
    o2[0xD] = c;
    o2[0xC] = c;
    o[0xE] = c;
    o[0xD] = c;
    o[0xC] = c;
    idx = *(s16 *)(o + 0x60) * 204 + 0x1000;
    *(u16 *)(o2 + 0x46) = idx;
    *(u16 *)(o + 0x46) = idx;
    q = o[0xC] << 5;
    *(u16 *)(o2 + 0x44) = q;
    *(u16 *)(o + 0x44) = q;
}
