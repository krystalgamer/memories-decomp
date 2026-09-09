#include "../types.h"
#include "../psyq/rand.h"
#include "ai.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_grid.h"
#include "duel_side_state.h"
#include "func_80026C0C.h"
#include "rand_get_interval.h"
#include "duel_field_equip_search.h"
#include "duel_card_checks.h"

/* The AI's turn-action searches, in call order: the spell search, the fusion
   search, and the pick that runs both. They are contiguous, share the
   gcc_2_8_1_g8_split profile, and all three write the same pending selection
   at D_800EAE88. */

/* The pending selection lives in ai.h. The spell search writes it as a
   record; the other two write single bytes, so the byte view below is the
   same object under an alias and each function keeps the spelling its own
   match needs. */
extern AiSelection D_800EAE88;
extern u8 D_800EAE88_bytes[] asm("D_800EAE88");

/* The spell search reads the record's slot byte signed; DuelCardRecord spells
   the same byte unsigned, so it keeps its own view of the collected entries
   and casts at the call. */
extern s32 func_80027060(void);

int func_8002712C(void)
{
 DuelCardRecord*list[6];int count=Duel_CollectFieldCardsByType(list,0,CARD_TYPE_MAGIC);
 if(count){s8 first=list[0]->table_index;int index;D_800EAE88.result=first%DUEL_FIELD_ROW_SIZE+11;D_800EAE88.field1=0;D_800EAE88.zero=0;D_800EAE88.random=0;D_800EAE88.value=6;
  index=func_80026C0C(DUEL_FIELD_SIDE_ZONE_COUNT);if(index>=0){D_800EAE88.random=rand()&1;D_800EAE88.value+=index%DUEL_FIELD_ROW_SIZE;}return 0;}return 1;
}

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

    st = D_800EAE88_bytes;
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

    st = D_800EAE88_bytes;
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

/* AI turn action pick: tries the equip, trap and spell searches at random
   odds, then picks a random card from the side's back-row records and fills
   the D_800EAE88 selection with a field slot for it: a monster goes to the
   front row with a random flag, anything else to the back row, forcing the
   flag on for an equip. Always returns 0. */
s32 func_80027508(void) {
    DuelCardRecord *card;
    s32 slot;

    if ((rand() & 1) == 0 && func_80026DC8() == 0) {
        return 0;
    }
    if ((rand() & 1) == 0 && func_80027060() == 0) {
        return 0;
    }
    if ((rand() & 3) == 0 && func_8002712C() == 0) {
        return 0;
    }
    if (func_80027228() == 0) {
        return 0;
    }
    card = &D_801A7AD8[
        D_8009B1D5 * DUEL_CARD_SIDE_RECORD_COUNT +
        Rand_GetInterval(DUEL_FIELD_ROW_SIZE)
    ];
    if (((gDuel_adwCardStats[*(s16 *)&card->card_id - 1] >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK) < CARD_TYPE_MAGIC) {
        s8 v;

        slot = func_80026C0C(DUEL_FIELD_ROW_SIZE);
        if (slot < 0) {
            slot = Rand_GetInterval(DUEL_FIELD_ROW_SIZE);
        }
        v = card->table_index;
        D_800EAE88_bytes[1] = 0;
        D_800EAE88_bytes[6] = slot % DUEL_FIELD_ROW_SIZE + 1;
        D_800EAE88_bytes[0] = v % DUEL_FIELD_ROW_SIZE + 0xB;
        D_800EAE88_bytes[7] = rand() & 1;
        D_800EAE88_bytes[8] = 1;
    } else {
        s8 v;

        slot = func_80026C0C(DUEL_FIELD_SIDE_ZONE_COUNT);
        if (slot < 0) {
            slot = Rand_GetInterval(DUEL_FIELD_ROW_SIZE);
        }
        v = card->table_index;
        D_800EAE88_bytes[1] = 0;
        D_800EAE88_bytes[7] = 0;
        D_800EAE88_bytes[6] = slot % DUEL_FIELD_ROW_SIZE + 6;
        D_800EAE88_bytes[0] = v % DUEL_FIELD_ROW_SIZE + 0xB;
        D_800EAE88_bytes[8] = rand() & 1;
        if (((gDuel_adwCardStats[*(s16 *)&card->card_id - 1] >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK) == CARD_TYPE_EQUIP) {
            D_800EAE88_bytes[8] = 1;
        }
    }
    return 0;
}
