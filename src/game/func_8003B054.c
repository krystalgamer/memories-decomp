#define D_8009B0D8_IS_HALFWORD
#include "../types.h"
#include "graphics_frame.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object_api.h"
#include "display_object_helpers.h"

extern s32 func_80039F1C(void);
extern void func_80040510(u8 *, s32, s32, s32, s32, s32, s32, s32, s32, s32);
extern void func_80039FD4(u8 *);

void func_8003B054(u8 *p)
{
    u8 *o;
    u8 *o2;
    s32 id;
    s32 idx;
    s32 c;
    s32 q;

    if (func_80039F1C() == 0) {
        if (*(s16 *)(p + 0x40) != 0) {
            o = *(u8 **)p;
            *(u16 *)(o + 0x60) = 0;
        } else {
            id = *(s8 *)(p + 0x30);
            idx = id - 0x41;
            o = func_800400AC(func_8004002C(), 1);
            func_80040510(o, *(s16 *)(p + 0x34), *(s16 *)(p + 0x36), 0x30, 0x30, 0, 0, 0xE, 0x380, 0xF0);
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
        func_80040510(o2, *(s16 *)(o + 0x30), *(s16 *)(o + 0x32), 0x30, 0x30, o[0x5C], o[0x5D], 0xE, 0x200, 0xFD);
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
        *(u16 *)(o + 0x60) += D_8009B0D8;
    } else {
        *(u16 *)(o + 0x60) -= D_8009B0D8;
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
