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
#include "display_object_position.h"
#include "menu_record.h"
#include "display_effect_update_callbacks.h"

extern u16 D_8009B0D8_halfword asm("D_8009B0D8");

void func_8003AD6C(MenuRecord *p)
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
    DisplayObject *o;

    if (func_80039F1C((DisplayEffectState *)p) == 0) {
        p->field_32 |= 0x10;
        r = *(u8 **)&p->grid[0][0];
        a = r[0x67];
        b = *(s8 *)(r + 0x16);
        func_80039F90((void **)p->grid[0]);
        func_8003A1EC((u8 *)p, (u8 **)p->grid[0], p->field_31);
        func_8003A440((u8 **)p->grid[0], (GsALON | GsAONE), b);
        func_8003A1EC((u8 *)p, (u8 **)p->grid[1], p->field_31);
        d = b - 1;
        func_8003A440((u8 **)p->grid[1], (GsALON | GsATWO), d);
        func_8003A1EC((u8 *)p, (u8 **)p->grid[2], a);
        func_8003A440((u8 **)p->grid[2], (GsALON | GsAONE), b);
        func_8003A1EC((u8 *)p, (u8 **)p->grid[3], a);
        func_8003A440((u8 **)p->grid[3], (GsALON | GsATWO), d);
        p->field_40 = 0x80;
    }
    n = (u16)p->field_40 - D_8009B0D8 * 8;
    p->field_40 = n;
    if (n <= 0) {
        p->display_effect_step = 0;
        func_8003A440((u8 **)p->grid[0], 0,
                      *(s8 *)(*(u8 **)&p->grid[0][0] + 0x16));
        func_8003A920((DisplayPositionGroup *)p, *(s16 *)&p->field_34,
                      *(s16 *)&p->field_36);
        func_80039F90((void **)p->grid[1]);
        func_80039F90((void **)p->grid[2]);
        func_80039F90((void **)p->grid[3]);
        p->field_32 &= 0xEF;
        return;
    }
    c = n;
    c |= (c << 8) | (c << 0x10);
    for (m = 2; m >= 0; m--) {
        o = *(DisplayObject **)&p->grid[2][m];
        if (o != 0) {
            o->field_0C = c;
        }
        o = *(DisplayObject **)&p->grid[3][m];
        if (o != 0) {
            o->field_0C = c;
        }
    }
    c = 0x80 - p->field_40;
    c |= (c << 8) | (c << 0x10);
    for (m = 2; m >= 0; m--) {
        o = *(DisplayObject **)&p->grid[0][m];
        if (o != 0) {
            o->field_0C = c;
        }
        o = *(DisplayObject **)&p->grid[1][m];
        if (o != 0) {
            o->field_0C = c;
        }
    }
    dd[0] = p->field_40 / 8;
    w = (0x80 - p->field_40) / 8;
    dd[1] = -w;
    if (p->field_3C == 0) {
        dd[0] = -dd[0];
        dd[1] = w;
    }
    y = *(s16 *)&p->field_36;
    x = *(s16 *)&p->field_34 + dd[0];
    x = (s16)x;
    func_8003A920((DisplayPositionGroup *)p, x, y);
    func_8003A920((DisplayPositionGroup *)p->grid[1], x, y);
    x = *(s16 *)&p->field_34 - dd[1];
    x = (s16)x;
    func_8003A920((DisplayPositionGroup *)p->grid[2], x, y);
    func_8003A920((DisplayPositionGroup *)p->grid[3], x, y);
}

void func_8003B054(u8 *p)
{
    DisplayObject *o;
    DisplayObject *o2;
    s32 id;
    s32 idx;
    s32 c;
    s32 q;

    if (func_80039F1C((DisplayEffectState *)p) == 0) {
        if (*(s16 *)(p + 0x40) != 0) {
            o = *(DisplayObject **)p;
            *(u16 *)&o->field_60 = 0;
        } else {
            id = *(s8 *)(p + 0x30);
            idx = id - 0x41;
            o = func_800400AC(func_8004002C(), 1);
            func_80040510((DisplayObjectConfigView *)o, *(s16 *)(p + 0x34), *(s16 *)(p + 0x36), 0x30, 0x30, 0, 0, 0xE, 0x380, 0xF0);
            *(u16 *)&o->field_40.h.field_40 += (idx >> 4) << 6;
            *(u8 *)&o->field_5C = (idx % 5) * 48;
            ((u8 *)&o->field_5C)[1] = (idx / 5) * 48;
            *(u16 *)&o->field_40.h.field_42 += idx % 16;
            *(u16 *)&o->field_44.h.field_46 = 0;
            o->field_0C = 0;
            o->attribute |= 0x51000000;
            func_80042918(o);
            func_800428EC((u8 *)o, -8);
            *(DisplayObject **)p = o;
            *(u16 *)&o->field_60 = 0x14;
        }
        o->attribute = (o->attribute | (GsALON | GsAONE)) & ~GsROTOFF;
        *(u16 *)&o->field_48.h.field_4A = 0;
        o2 = func_800400AC(func_8004002C(), 1);
        func_80040510((DisplayObjectConfigView *)o2, *(s16 *)&o->field_30.h.field_30, *(s16 *)&o->field_30.h.field_32, 0x30, 0x30, *(u8 *)&o->field_5C, ((u8 *)&o->field_5C)[1], 0xE, 0x200, 0xFD);
        o2->attribute = (o2->attribute | 0x61000000) & ~GsROTOFF;
        *(u16 *)&o2->field_44.h.field_46 = *(u16 *)&o->field_44.h.field_46;
        *(u16 *)&o2->field_48.h.field_4A = 0;
        func_80042918(o2);
        func_800428EC((u8 *)o2, -9);
        *(DisplayObject **)(p + 4) = o2;
    }
    o = *(DisplayObject **)p;
    o2 = *(DisplayObject **)(p + 4);
    if (*(s16 *)(p + 0x40) != 0) {
        *(u16 *)&o->field_60 += D_8009B0D8_halfword;
    } else {
        *(u16 *)&o->field_60 -= D_8009B0D8_halfword;
    }
    if (o->field_60 <= 0) {
        o->attribute = (o->attribute & ~(GsALON | GsATWO | GsAONE)) | GsROTOFF;
        o->field_0C = 0x808080;
        *(u16 *)&o->field_44.h.field_46 = 0x1000;
        func_8004036C(o2);
        *(DisplayObject **)(p + 4) = 0;
        goto clear;
    }
    if (o->field_60 >= 0x14) {
        func_80039FD4(p);
    clear:
        p[0x33] = 0;
        return;
    }
    c = -0x80 - o->field_60 * 6;
    ((u8 *)&o2->field_0C)[2] = c;
    ((u8 *)&o2->field_0C)[1] = c;
    *(u8 *)&o2->field_0C = c;
    ((u8 *)&o->field_0C)[2] = c;
    ((u8 *)&o->field_0C)[1] = c;
    *(u8 *)&o->field_0C = c;
    idx = o->field_60 * 204 + 0x1000;
    *(u16 *)&o2->field_44.h.field_46 = idx;
    *(u16 *)&o->field_44.h.field_46 = idx;
    q = *(u8 *)&o->field_0C << 5;
    *(u16 *)&o2->field_44.h.field_44 = q;
    *(u16 *)&o->field_44.h.field_44 = q;
}
