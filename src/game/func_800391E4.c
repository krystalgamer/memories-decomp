#include "../types.h"
#include "../unmatched.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "display_object_api.h"
#include "display_object_layout.h"
#include "display_object_helpers.h"
#include "duel_effect.h"
#include "text_box_runtime.h"

void func_800391E4(DuelEffectChannel *p) {
    DisplayObject *e;
    s32 v;
    s32 b;
    s32 f;

    e = p->field_28;
    if (e == (DisplayObject *)0) {
        e = func_800400AC(func_8004006C(), 6);
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

    func_80042918(e);
    func_800428EC((u8 *)e, *(s8 *)&p->field_59);

    *(s16 *)&e->field_30.h.field_30 = *(u16 *)&p->field_3C;
    *(s16 *)&e->field_30.h.field_32 = *(u16 *)&p->field_40;
    *(s16 *)&e->field_3C.h.field_3C = *(u16 *)&p->field_3E;
    *(s16 *)&e->field_3C.h.field_3E = *(u16 *)&p->field_42;
    v = (s16)*(u16 *)&p->field_3E / 2;
    *(s16 *)&e->field_18 = v;
    e->field_48.h.field_48 = v;
    v = (s16)*(u16 *)&p->field_42 / 2;
    *(s16 *)&e->field_1A = v;
    e->field_48.h.field_4A = v;

    if ((p->flags_34 & 0x20) != 0) {
        if (p->field_2C != 0) {
            func_8004036C(p->field_2C);
        }
        e = func_800400AC(func_8004002C(), 4);
        func_800427DC(e, 1);
        p->field_2C = e;
        e->attribute = e->attribute | (GsALON | GsATWO);
        func_80042918(e);
        func_800428EC((u8 *)e, (s8)(p->field_59 - 1));
        *(s32 *)&e->field_54 = 0xA0A0A0;
        e->field_4C = 0xA0A0A0;
        e->field_34 = 0xA0A0A0;
        e->field_2C.word = 0xA0A0A0;
        e->field_44.word = 0x808080;
        e->field_3C.word = 0x808080;
        func_80039140((u8 *)p);
    }
}
