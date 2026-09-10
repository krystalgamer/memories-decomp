#include "../types.h"
#include "display_object.h"
#include "duel_effect.h"
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
