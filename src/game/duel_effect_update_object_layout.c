#include "../types.h"
#include "duel_effect.h"
#include "display_object.h"
#include "duel_effect_entry_ranges.h"
#include "duel_effect_update_object_layout.h"
#include "../unmatched.h"
#include "dialog_choice.h"

/* Lays out the choice widget's six x/y pairs on the display object the
   channel holds at 0x30. The pairs sit at stride 8 from 0x28; three of them
   take the channel's field_3C as x and three take field_3C + field_3E, and
   the y values come from the entry's field_18 modulo ten, which selects the
   dialog's row spacing. The record is the canonical DisplayObject:
   dialog_update_choice.c stores one there, through the same 0x30. */
void DuelEffect_UpdateObjectLayout(DuelEffectChannel *p) {
    DisplayObject *q;
    s32 n;
    s32 m;
    u8 k;
    s32 a;
    s32 t;
    s16 t0;
    s16 t2;
    s32 b;
    s32 c;
    s32 v;
    s32 v0;
    s32 v2;
    s32 w;
    s32 x;
    s32 y;

    q = (DisplayObject *)p->field_30;
    n = *(u16 *)&p->field_3C;
    q->field_48.h.field_48 = n;
    q->field_38.h.field_38 = n;
    q->position.h.field_28 = n;
    m = *(u16 *)&p->field_3C + *(u16 *)&p->field_3E;
    q->field_50.h.field_50 = m;
    q->field_40.h.field_40 = m;
    q->field_30.h.field_30 = m;

    k = D_800EB288[
        gDuelEffect_awEntryRangeBoundaries[p->index_57]
    ].field_18 % 10;

    if (k == 1) {
        t = D_8009B34C & 0x30;
        a = t / 16 * 0x10;
        v = *(u16 *)&p->field_40 + a + gDialog_bChoice * 0x10;
        q->position.h.field_2A = v;

        do {
            w = v;
        } while (0);
        q->field_30.h.field_32 = v;
        v = v + 0x10;
        w = w + 0x10;
        q->field_40.h.field_42 = v;
        q->field_38.h.field_3A = v;
        q->field_50.h.field_52 = w;
        q->field_48.h.field_4A = w;
        return;
    }

    if (k == 2) {
        t2 = D_8009B34C & 0x30;
        b = t2 / 16 * 0xC;
        v2 = *(u16 *)&p->field_40 + b + gDialog_bChoice * 0xC;
        x = v2 - 2;
        y = v2 + 6;
    } else if (k == 0) {
        t0 = D_8009B34C & 0x30;
        c = t0 / 16 * 0xC;
        v0 = *(u16 *)&p->field_40 + c + gDialog_bChoice * 0xC;
        x = v0 + 2;
        y = v0 + 0xA;
    } else {
        return;
    }

    q->field_30.h.field_32 = x;
    q->position.h.field_2A = x;
    x = x + 0xC;
    q->field_40.h.field_42 = y;
    q->field_38.h.field_3A = y;
    q->field_50.h.field_52 = x;
    q->field_48.h.field_4A = x;
}
