/* Seven contiguous display-effect position and update callbacks on the same
 * MenuRecord / DisplayEffectState storage.
 *
 * The final four scale a motion delta by the per-frame step multiplier and do
 * not agree on its width. graphics_frame.h declares D_8009B0D8 once, as s32,
 * and the disagreement is spelled at the use sites: func_8003AAE4,
 * func_8003AC48 and func_8003AD6C read it plain, and func_8003B054 reads it
 * `(u16)` twice, which is the halfword load its retail code does. A unit that
 * needs the reload rather than a width selects that for the whole unit --
 * display_object_fade_callbacks.c defines D_8009B0D8_IS_VOLATILE. */
#include "../types.h"
#include "display_object_config.h"
#include "display_effect_lifecycle.h"
#include "campaign_scene_package.h"
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
#include "trig_constants.h"
#include "func_8003A990.h"
#include "func_8003AAE4.h"
#include "func_8003AC48.h"
#include "display_effect_update_callbacks.h"

void func_8003A920(
    DisplayPositionGroup *group,
    s16 x,
    s16 y
)
{
    s32 i;

    for (i = 2; i >= 0; i--) {
        if (group->children[i] != 0) {
            group->children[i]->x = x;
            group->children[i]->y = y;
        }
    }
}

void func_8003A95C(DisplayPositionGroup *group, s32 x, s32 y)
{
    group->x = x;
    group->y = y;
    func_8003A920(group, (s16)x, (s16)y);
}

/* Eases one display-effect record from its 0x34/0x36 position to the
   0x40/0x42 destination over a quarter turn of cosine, then clears the step
   byte. The record is a MenuRecord, the element type of D_800EB010;
   display_effect_step_table.c hands this callback a u8 *, so the parameter
   stays that and the record is taken through a local.

   Every use goes through that local, including the two calls that want a
   u8 * again. That is not tidiness: leaving `p` live alongside `r` makes
   GCC 2.8.1 hold both in callee-saved registers, which grows the frame by
   eight bytes and the function with it. One name, one register. */
void func_8003A990(u8 *p)
{
    MenuRecord *r = (MenuRecord *)p;
    s32 d;
    s32 t;
    s32 c;
    s32 dx;
    s32 dy;

    if (func_80039F1C((DisplayEffectState *)r) == 0) {
        r->field_48 = TRIG_ANGLE_QUARTER_TURN;
        d = TRIG_ANGLE_QUARTER_TURN / r->field_44;
        r->field_4A = d;
        if (d >= 0) {
            r->field_48 = 0;
        }
        r->field_44 = *(u16 *)&r->field_40 - r->field_34;
        r->field_46 = *(u16 *)&r->field_42 - r->field_36;
    }

    t = *(u16 *)&r->field_48 + *(u16 *)&r->field_4A;
    r->field_48 = t;

    if ((u16)(t - 1) >= TRIG_ANGLE_QUARTER_TURN - 1) {
        func_8003A95C((DisplayPositionGroup *)r, r->field_40, r->field_42);
        r->display_effect_step = 0;
    } else {
        c = rcos((s16)t);
        dx = c * r->field_44 / ONE;
        dy = c * r->field_46 / ONE;
        if (r->field_4A < 0) {
            dx = r->field_44 - dx;
            dy = r->field_46 - dy;
        }
        func_8003A95C(
            (DisplayPositionGroup *)r,
            (s16)(*(u16 *)&r->field_40 - dx),
            (s16)(*(u16 *)&r->field_42 - dy)
        );
    }
}

void func_8003AAE4(MenuRecord *p) {
    u8 *q;
    s32 *e;
    s32 *c;
    s32 a;
    s32 b;
    s32 v;
    s32 m;
    s32 i;

    if (func_80039F1C((DisplayEffectState *)p) == 0) {
        *(s16 *)&p->field_34 = 0x68;
        p->field_32 |= 0x10;
        if (p->field_3C != 0) {
            *(s16 *)&p->field_34 = 0xD8;
        }
        func_8003A920((DisplayPositionGroup *)p, *(s16 *)&p->field_34,
                      *(s16 *)&p->field_36);
        q = *(u8 **)&p->grid[0][0];
        a = *(s8 *)(q + 0x16);
        b = q[0x67];
        func_8003A440((u8 **)p->grid[0], (GsALON | GsAONE), a);
        e = p->grid[1];
        func_8003A1EC(p, (u8 **)e, b);
        func_8003A440((u8 **)e, (GsALON | GsATWO), a - 1);
        p->field_40 = 0;
    }

    v = (u16)p->field_40 + D_8009B0D8 * 8;
    p->field_40 = v;

    if (p->field_40 >= 0x80) {
        p->display_effect_step = 0;
        func_8003A440((u8 **)p->grid[0], 0,
                      *(s8 *)(*(u8 **)&p->grid[0][0] + 0x16));
        func_80039F90((void **)p->grid[1]);
        p->field_32 &= 0xEF;
    } else {
        m = p->field_40;
        m |= (m << 8) | (m << 16);
        for (i = 2, c = &p->grid[0][2]; i >= 0; i--, c--) {
            if (*(u8 **)c != 0) {
                *(s32 *)(*(u8 **)c + 0xC) = m;
            }
            if (*(u8 **)(c + 3) != 0) {
                *(s32 *)(*(u8 **)(c + 3) + 0xC) = m;
            }
        }
    }
}

void func_8003AC48(MenuRecord *p)
{
    DisplayObject *h;
    u8 **d;
    DisplayObject *e;
    s32 x;
    s32 y;
    s32 t;
    s32 u;
    s32 m;
    s32 i;

    if (func_80039F1C((DisplayEffectState *)p) == 0) {
        p->field_32 |= 0x10;
        h = (DisplayObject *)p->grid[0][0];
        x = h->field_16;
        y = h->field_67;
        func_8003A440((u8 **)p->grid[0], (GsALON | GsAONE), x);
        d = (u8 **)p->grid[1];
        func_8003A1EC(p, d, y);
        func_8003A440(d, (GsALON | GsATWO), x - 1);
        p->field_40 = 0x80;
    }

    t = *(u16 *)&p->field_40 - (D_8009B0D8 << 3);
    p->field_40 = t;
    u = (s16)t;
    if (u <= 0) {
        p->display_effect_step = 0;
        func_80039F90((void **)p->grid[1]);
        func_80039FD4((u8 *)p);
    } else {
        m = u;
        m |= (m << 8) | (m << 16);
        for (i = 2; i >= 0; i--) {
            e = (DisplayObject *)p->grid[0][i];
            if (e != 0) {
                e->field_0C = m;
            }
            e = (DisplayObject *)p->grid[1][i];
            if (e != 0) {
                e->field_0C = m;
            }
        }
    }
}

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
        func_8003A1EC(p, (u8 **)p->grid[0], p->field_31);
        func_8003A440((u8 **)p->grid[0], (GsALON | GsAONE), b);
        func_8003A1EC(p, (u8 **)p->grid[1], p->field_31);
        d = b - 1;
        func_8003A440((u8 **)p->grid[1], (GsALON | GsATWO), d);
        func_8003A1EC(p, (u8 **)p->grid[2], a);
        func_8003A440((u8 **)p->grid[2], (GsALON | GsAONE), b);
        func_8003A1EC(p, (u8 **)p->grid[3], a);
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
            idx = id - CAMPAIGN_DIALOG_PORTRAIT_FIRST_EFFECT_ID;
            o = func_800400AC(func_8004002C(), 1);
            func_80040510((DisplayObjectConfigView *)o, *(s16 *)(p + 0x34), *(s16 *)(p + 0x36), 0x30, 0x30, 0, 0, 0xE, 0x380, 0xF0);
            *(u16 *)&o->field_40.h.field_40 += (idx >> 4) << 6;
            *(u8 *)&o->field_5C =
                (idx % CAMPAIGN_DIALOG_PORTRAIT_GRID_COLUMN_COUNT) *
                CAMPAIGN_DIALOG_PORTRAIT_IMAGE_WIDTH;
            ((u8 *)&o->field_5C)[1] =
                (idx / CAMPAIGN_DIALOG_PORTRAIT_GRID_COLUMN_COUNT) *
                CAMPAIGN_DIALOG_PORTRAIT_IMAGE_HEIGHT;
            *(u16 *)&o->field_40.h.field_42 +=
                idx % CAMPAIGN_DIALOG_PORTRAIT_CLUT_ROWS_PER_COLUMN;
            *(u16 *)&o->field_44.h.field_46 = 0;
            o->field_0C = 0;
            o->attribute |=
                GsALON | GsAONE | DISPLAY_OBJECT_ATTRIBUTE_8BPP;
            func_80042918(o);
            func_800428EC((u8 *)o, -8);
            *(DisplayObject **)p = o;
            *(u16 *)&o->field_60 = 0x14;
        }
        o->attribute = (o->attribute | (GsALON | GsAONE)) & ~GsROTOFF;
        *(u16 *)&o->field_48.h.field_4A = 0;
        o2 = func_800400AC(func_8004002C(), 1);
        func_80040510((DisplayObjectConfigView *)o2, *(s16 *)&o->field_30.h.field_30, *(s16 *)&o->field_30.h.field_32, 0x30, 0x30, *(u8 *)&o->field_5C, ((u8 *)&o->field_5C)[1], 0xE, 0x200, 0xFD);
        o2->attribute = (o2->attribute |
            GsALON | GsATWO | DISPLAY_OBJECT_ATTRIBUTE_8BPP) & ~GsROTOFF;
        *(u16 *)&o2->field_44.h.field_46 = *(u16 *)&o->field_44.h.field_46;
        *(u16 *)&o2->field_48.h.field_4A = 0;
        func_80042918(o2);
        func_800428EC((u8 *)o2, -9);
        *(DisplayObject **)(p + 4) = o2;
    }
    o = *(DisplayObject **)p;
    o2 = *(DisplayObject **)(p + 4);
    if (*(s16 *)(p + 0x40) != 0) {
        *(u16 *)&o->field_60 += (u16)D_8009B0D8;
    } else {
        *(u16 *)&o->field_60 -= (u16)D_8009B0D8;
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
