/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8_split_no_strength_reduce this
 * source rebuilt the target byte for byte, but only by
 * pinning 3 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/func_80060E70.c.
 */
#define GDUEL_WSELECTEDCARDID_IN_DATA
#include "../types.h"
#include "../game/card_constants.h"
#include "../game/card_list_rows.h"
#include "../game/duel_effect.h"
#include "../game/text_box_lifecycle.h"
#include "../game/text_box_runtime.h"
#include "../game/duel_card.h"
#include "../game/text_staging.h"

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
    TextStagingValues *dst;
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
            D_801D5608[0].card.card_id = id;
            dst->card.count = q[0];
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
        func_80039A14((struct DuelEffectChannel *)box);
        q += 2;
        e += 2;
        w = (CardListRowSet *)((u8 *)w + 2);
        y += 0x16;
    }
}
