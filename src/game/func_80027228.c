#include "../types.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_grid.h"
#include "../psyq/rand.h"

extern s32 Duel_CollectFieldCardsBelowType(DuelCardRecord **, s32, s32);
extern s32 Duel_CollectFieldCardsByType(DuelCardRecord **, s32, s32);
extern s32 Duel_CheckFusion(s32, s32);
extern s32 func_80026C0C(s32);
extern s32 Rand_GetInterval(s32);

extern u8 D_800EAE88[];

/* AI fusion attempt. Collects the side's monsters, rolls a coin, and looks
   for a fusable pair between the monsters and the hand (first pass) or,
   when a field slot is free, between two monsters of different slots
   (second pass). A hit fills the D_800EAE88 selection with the two slot
   bytes and the target slot; returns 0 on a selection, 1 otherwise. */
s32 func_80027228(void) {
    DuelCardRecord *list1[6];
    DuelCardRecord *list0[6];
    DuelCardRecord *e1;
    DuelCardRecord *e2;
    s32 count;
    s32 slot;
    s32 i;
    s32 j;

    count = Duel_CollectFieldCardsBelowType(list0, 0, CARD_TYPE_MAGIC);
    if (count == 0) {
        return 1;
    }
    count--;
    if (count != 0) {
        Rand_GetInterval(count + 1);
    }
    if ((rand() & 1) == 0) {
        return 1;
    }
    Duel_CollectFieldCardsByType(list1, DUEL_FIELD_ROW_SIZE, -1);
    for (i = 0; (e1 = list0[i]) != 0; i++) {
        for (j = 0; (e2 = list1[j]) != 0; j++) {
            if (Duel_CheckFusion(*(s16 *)&e1->card_id, *(s16 *)&e2->card_id)) {
                goto found1;
            }
        }
    }
    slot = func_80026C0C(DUEL_FIELD_ROW_SIZE);
    if (slot >= 0) {
        goto phase2;
    }
    return 1;

found1:
    {
    u8 *st;
    s8 v;

    st = D_800EAE88;
    v = e1->table_index;
    st[1] = 0;
    st[0] = v % DUEL_FIELD_ROW_SIZE + 0xB;
    v = e2->table_index;
    st[6] = v % DUEL_FIELD_ROW_SIZE + 1;
    st[7] = rand() & 1;
    st[8] = 0;
    return 0;
    }

found2:
    {
    u8 *st;
    s8 v;
    s8 w;

    st = D_800EAE88;
    v = e1->table_index;
    st[0] = v % DUEL_FIELD_ROW_SIZE + 0xB;
    w = e2->table_index;
    st[2] = 0;
    st[6] = slot % DUEL_FIELD_ROW_SIZE + 1;
    st[1] = w % DUEL_FIELD_ROW_SIZE + 0xB;
    st[7] = rand() & 1;
    st[8] = 0;
    return 0;
    }

phase2:
    Duel_CollectFieldCardsBelowType(list1, 0, CARD_TYPE_MAGIC);
    for (i = 0; (e1 = list0[i]) != 0; i++) {
        for (j = 0; (e2 = list1[j]) != 0; j++) {
            if (*(s8 *)&e1->table_index != *(s8 *)&e2->table_index &&
                Duel_CheckFusion(*(s16 *)&e1->card_id, *(s16 *)&e2->card_id)) {
                goto found2;
            }
        }
    }
    return 1;
}
