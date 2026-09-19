#include "../types.h"
#include "../unmatched.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "display_object_core.h"
#include "display_object_layout.h"
#include "display_object_helpers.h"
#include "color_constants.h"
#include "duel_effect.h"
#include "func_80035E20.h"
#include "text_box_runtime.h"

/* Lays the text box's second object out as a six-point outline around the
 * first: the top corners 8 above the box, the side points 8 outside it at
 * its half height, and the bottom corners 8 below it. The six x/y pairs are
 * the stride-8 run from position that display_object.h describes for
 * DuelEffect_UpdateObjectLayout. */
void func_80039140(struct DuelEffectChannel *record) {
    DisplayObject *a = record->field_28, *b = record->field_2C;
    u16 v;
    b->field_18 = a->field_18;
    b->field_1A = a->field_1A;
    v = a->field_30.h.field_30;
    b->field_48.h.field_48 = v;
    b->position.h.field_28 = v;
    b->field_38.h.field_38 = v - 8;
    v = a->field_30.h.field_30 + a->field_3C.h.field_3C;
    b->field_50.h.field_50 = v;
    b->field_30.h.field_30 = v;
    b->field_40.h.field_40 = v + 8;
    v = a->field_30.h.field_32 - 8;
    b->field_30.h.field_32 = v;
    b->position.h.field_2A = v;
    v = a->field_30.h.field_32 + a->field_48.h.field_4A;
    b->field_40.h.field_42 = v;
    b->field_38.h.field_3A = v;
    v = a->field_30.h.field_32 + a->field_3C.h.field_3E + 8;
    b->field_50.h.field_52 = v;
    b->field_48.h.field_4A = v;
}

void func_800391E4(DuelEffectChannel *p) {
    DisplayObject *e;
    s32 v;
    s32 b;
    s32 f;

    e = p->field_28;
    if (e == (DisplayObject *)0) {
        e = DisplayObject_AcquireSlot(DisplayObject_FindFreeSlot(), 6);
        b = p->index_57;
        e->field_40.h.field_40 = 0x280;
        e->field_40.h.field_42 = 0xE8;
        e->field_66 = 0xA;
        f = e->flags;
        e->field_4C = (s32)func_80035E20;
        e->field_67 = b;
        e->flags = f | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        p->field_28 = e;
    }

    DisplayObject_SelectOrderingTable1(e);
    DisplayObject_SetDepthOffset(e, (s8)p->field_59);

    e->field_30.h.field_30 = (u16)p->field_3C;
    e->field_30.h.field_32 = (u16)p->field_40;
    e->field_3C.h.field_3C = (u16)p->field_3E;
    e->field_3C.h.field_3E = (u16)p->field_42;
    v = p->field_3E / 2;
    e->field_18 = v;
    e->field_48.h.field_48 = v;
    v = p->field_42 / 2;
    e->field_1A = v;
    e->field_48.h.field_4A = v;

    if ((p->flags_34 & 0x20) != 0) {
        if (p->field_2C != 0) {
            DisplayObject_ReleaseIfPresent(p->field_2C);
        }
        e = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 4);
        DisplayObject_InitializeGouraudQuad(e, 1);
        p->field_2C = e;
        e->attribute = e->attribute | (GsALON | GsATWO);
        DisplayObject_SelectOrderingTable1(e);
        DisplayObject_SetDepthOffset(e, (s8)(p->field_59 - 1));
        *(s32 *)&e->field_54 = 0xA0A0A0;
        e->field_4C = 0xA0A0A0;
        e->field_34.word = 0xA0A0A0;
        e->field_2C.word = 0xA0A0A0;
        e->field_44.word = COLOR_RGB24_NEUTRAL_GREY;
        e->field_3C.word = COLOR_RGB24_NEUTRAL_GREY;
        func_80039140(p);
    }
}
