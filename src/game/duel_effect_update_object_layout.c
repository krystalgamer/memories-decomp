#include "../types.h"
#include "duel_effect.h"
#include "duel_effect_entry_state.h"
#include "display_object.h"
#include "duel_effect_entry_ranges.h"
#include "duel_effect_update_object_layout.h"
#include "../unmatched.h"
#include "dialog_choice_control.h"
#include "dialog_choice.h"

#define DUEL_EFFECT_UNSIGNED_HALFWORD(field) (*(u16 *)&(field))

/* Lays out the choice widget's six x/y pairs on the display object the
   channel holds at 0x30. The pairs sit at stride 8 from 0x28; three of them
   take the channel's rect_x as x and three take rect_x + rect_width, and
   the y values come from the entry's field_18 modulo ten, which selects the
   dialog's row spacing. The record is the canonical DisplayObject:
   Dialog_UpdateChoice stores one there, through the same 0x30. */
#ifndef DUEL_EFFECT_LAYOUT_ROW_STEP
#define DUEL_EFFECT_LAYOUT_ROW_STEP 8
#endif

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
    n = DUEL_EFFECT_UNSIGNED_HALFWORD(p->rect_x);
    q->field_48.h.field_48 = n;
    q->field_38.h.field_38 = n;
    q->position.h.field_28 = n;
    m = DUEL_EFFECT_UNSIGNED_HALFWORD(p->rect_x) +
        DUEL_EFFECT_UNSIGNED_HALFWORD(p->rect_width);
    q->field_50.h.field_50 = m;
    q->field_40.h.field_40 = m;
    q->field_30.h.field_30 = m;

#if defined(VERSION_JAPAN) || defined(VERSION_EUROPE)
    /* The Japanese and European builds have one layout for every entry: no
       field_18 % 10 selection, and one y step from the choice row (8 in the
       Japanese build, 10 in the European one, whose wrapper defines its own)
       rather than the US spacings below. */
    v = DUEL_EFFECT_UNSIGNED_HALFWORD(p->rect_y) + (gDialog_bChoiceControl & DIALOG_CHOICE_CONTROL_LAYOUT_MASK) +
        ((s32)(*(u8 *)&gDialog_bChoice << 24) >> 20);
    q->position.h.field_2A = v;

    do {
        w = v;
    } while (0);
    q->field_30.h.field_32 = v;
    v = v + DUEL_EFFECT_LAYOUT_ROW_STEP;
    w = w + 0x10;
    q->field_40.h.field_42 = v;
    q->field_38.h.field_3A = v;
    q->field_50.h.field_52 = w;
    q->field_48.h.field_4A = w;
#else
    k = gDuelEffect_aEntries[
        gDuelEffect_awEntryRangeBoundaries[p->index_57]
    ].field_18 % 10;

    if (k == 1) {
        t = gDialog_bChoiceControl & DIALOG_CHOICE_CONTROL_LAYOUT_MASK;
        a = t / 16 * 0x10;
        v = DUEL_EFFECT_UNSIGNED_HALFWORD(p->rect_y) +
            a + gDialog_bChoice * 0x10;
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
        t2 = gDialog_bChoiceControl & DIALOG_CHOICE_CONTROL_LAYOUT_MASK;
        b = t2 / 16 * 0xC;
        v2 = DUEL_EFFECT_UNSIGNED_HALFWORD(p->rect_y) +
             b + gDialog_bChoice * 0xC;
        x = v2 - 2;
        y = v2 + 6;
    } else if (k == 0) {
        t0 = gDialog_bChoiceControl & DIALOG_CHOICE_CONTROL_LAYOUT_MASK;
        c = t0 / 16 * 0xC;
        v0 = DUEL_EFFECT_UNSIGNED_HALFWORD(p->rect_y) +
             c + gDialog_bChoice * 0xC;
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
#endif
}
