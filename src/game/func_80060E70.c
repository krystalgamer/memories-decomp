#define GDUEL_WSELECTEDCARDID_IN_DATA
#include "../types.h"
#include "card_constants.h"
#include "card_list_rows.h"
#include "duel_effect.h"
#include "text_box_lifecycle.h"
#include "text_box_runtime.h"
#include "duel_card.h"
#include "text_staging.h"

/* Builds the seven card-list text boxes for one row set: each two-halfword
 * input row supplies a card ID and count, staged into D_801D5608 before its
 * box is created. A monster card uses the base style and anything from the
 * magic type up the next one; an empty count marks the box and the row's
 * entry, and every box after the first gets flag 0x40.
 *
 * The three staging stores sit in their own do { } while (0) block. loop.c
 * then sees a nested loop and leaves the gDuel_adwCardStats address to be
 * rebuilt on each pass, as retail does, rather than hoisting it into $fp;
 * that register goes to the staging pointer instead. */
void func_80060E70(u16 *e, s32 idx, s32 flag, s32 ignored)
{
    CardListRowSet *s;
    DuelEffectChannel *box;
    s32 i;
    s32 y;
    s32 style;
    s32 base;
    s32 id;
    u16 *q;
    CardListRowSet *w;
    TextStagingValues *dst;

    s = &D_801A8000[idx];
    base = 0x18;
    s->enabled = flag;
    s->object->field_68 = flag;
    if (flag != 0) {
        base = 0x1A;
    }
    i = 0;
    dst = D_801D5608;
    w = s;
    y = 0;
    q = e + 1;
    for (i = 0; i < 7; i++) {
        w->card_entries[0] = e[0];
        style = e[0];
        if (style != 0) {
            id = style;
            style = base;
            if (((gDuel_adwCardStats[id - 1] >> CARD_STAT_TYPE_SHIFT) &
                 CARD_STAT_TYPE_MASK) >= CARD_TYPE_MAGIC) {
                style = base + 1;
            }
            do {
                D_801D5608[0].card.card_id = id;
                dst->card.count = q[0];
                gDuel_wSelectedCardID = id;
            } while (0);
        }
        box = TextBox_Create(idx + 1, style, s->x, s->y, 0x120, 0xB0);
        box->field_3A = y;
        if (q[0] == 0) {
            box->field_54 = 4;
            w->card_entries[0] |= 0x8000;
        }
        if (i != 0) {
            box->flags_34 |= 0x40;
        }
        func_80039A14(box);
        q += 2;
        e += 2;
        w = (CardListRowSet *)((u8 *)w + 2);
        y += 0x16;
    }
}
