#define GDUEL_WSELECTEDCARDID_IN_DATA
#include "../types.h"
#include "card_constants.h"
#include "card_list_rows.h"
#include "duel_effect.h"
#include "text_box_lifecycle.h"
#include "text_box_runtime.h"
#include "duel_card.h"

extern s32 D_801D5608[];

void func_80060E70(u16 *e, s32 idx, s32 flag, s32 ignored)
{
    CardListRowSet *s;
    DuelEffectChannel *box;
    s32 i;
    s32 y;
    s32 style;
    s32 base;
    register s32 id asm("$3");
    u16 *q;
    CardListRowSet *w;
    s32 *dst;
    register s32 *stats asm("$8");
    register s32 offset asm("$2");

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
            offset = id - 1;
            offset <<= 2;
            stats = gDuel_adwCardStats;
            style = base;
            if (((*(s32 *)((u8 *)stats + offset) >> CARD_STAT_TYPE_SHIFT) &
                 CARD_STAT_TYPE_MASK) >= CARD_TYPE_MAGIC) {
                style = base + 1;
            }
            D_801D5608[0] = id;
            dst[1] = q[0];
            gDuel_wSelectedCardID = id;
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
        func_80039A14((u8 *)box);
        q += 2;
        e += 2;
        w = (CardListRowSet *)((u8 *)w + 2);
        y += 0x16;
    }
}
