#include "../types.h"
#include "../psyq/rand.h"
#include "ai.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_grid.h"
#include "duel_side_state.h"
#include "rand_get_interval.h"
#include "duel_field_equip_search.h"
#include "duel_card_checks.h"
#include "duel_check_quit_input.h"
#include "duel_card_selection.h"
#include "duel_battle_stats.h"
#define D_800EAE88_VISIBLE
#include "../unmatched.h"

#define DISPLAY_OBJECT_VIEW(object) ((DisplayObject *)(object))

/* The AI's turn-action run: spell and fusion searches feed the action pick
   through D_800EAE88, followed by the occupied and face-up field-target
   selectors and the final per-turn play decision that consumes them. All six
   functions are contiguous and share the gcc_2_8_1_g8_split profile. */

/* The pending selection lives in ai.h. The spell search writes it as a
   record; the other two write single bytes, so the byte view below is the
   same object under an alias and each function keeps the spelling its own
   match needs. */

/* The spell search reads the record's slot byte signed; DuelCardRecord spells
   the same byte unsigned, so it keeps its own view of the collected entries
   and casts at the call. */

int func_8002712C(void)
{
 DuelCardRecord*list[6];int count=Duel_CollectFieldCardsByType(list,0,CARD_TYPE_MAGIC);
 if(count){s8 first=list[0]->table_index;int index;D_800EAE88.result=first%DUEL_FIELD_ROW_SIZE+11;D_800EAE88.field1=0;D_800EAE88.zero=0;D_800EAE88.random=0;D_800EAE88.value=6;
  index=Duel_FindFreeFieldSlot(DUEL_FIELD_SIDE_ZONE_COUNT);if(index>=0){D_800EAE88.random=rand()&1;D_800EAE88.value+=index%DUEL_FIELD_ROW_SIZE;}return 0;}return 1;
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
    slot = Duel_FindFreeFieldSlot(DUEL_FIELD_ROW_SIZE);
    if (slot >= 0) {
        goto phase2;
    }
    return 1;

found1:
    {
    AiSelection *st;
    s8 v;

    st = &D_800EAE88;
    v = e1->table_index;
    st->field1 = 0;
    st->result = v % DUEL_FIELD_ROW_SIZE + 0xB;
    v = e2->table_index;
    st->value = v % DUEL_FIELD_ROW_SIZE + 1;
    st->zero = rand() & 1;
    st->random = 0;
    return 0;
    }

found2:
    {
    AiSelection *st;
    s8 v;
    s8 w;

    st = &D_800EAE88;
    v = e1->table_index;
    st->result = v % DUEL_FIELD_ROW_SIZE + 0xB;
    w = e2->table_index;
    st->field_02 = 0;
    st->value = slot % DUEL_FIELD_ROW_SIZE + 1;
    st->field1 = w % DUEL_FIELD_ROW_SIZE + 0xB;
    st->zero = rand() & 1;
    st->random = 0;
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

    if ((rand() & 1) == 0 && Duel_SelectEquipPair() == 0) {
        return 0;
    }
    if ((rand() & 1) == 0 && Duel_SelectTrapPlay() == 0) {
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

        slot = Duel_FindFreeFieldSlot(DUEL_FIELD_ROW_SIZE);
        if (slot < 0) {
            slot = Rand_GetInterval(DUEL_FIELD_ROW_SIZE);
        }
        v = card->table_index;
        D_800EAE88.field1 = 0;
        D_800EAE88.value = slot % DUEL_FIELD_ROW_SIZE + 1;
        D_800EAE88.result = v % DUEL_FIELD_ROW_SIZE + 0xB;
        D_800EAE88.zero = rand() & 1;
        D_800EAE88.random = 1;
    } else {
        s8 v;

        slot = Duel_FindFreeFieldSlot(DUEL_FIELD_SIDE_ZONE_COUNT);
        if (slot < 0) {
            slot = Rand_GetInterval(DUEL_FIELD_ROW_SIZE);
        }
        v = card->table_index;
        D_800EAE88.field1 = 0;
        D_800EAE88.zero = 0;
        D_800EAE88.value = slot % DUEL_FIELD_ROW_SIZE + 6;
        D_800EAE88.result = v % DUEL_FIELD_ROW_SIZE + 0xB;
        D_800EAE88.random = rand() & 1;
        if (((gDuel_adwCardStats[*(s16 *)&card->card_id - 1] >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK) == CARD_TYPE_EQUIP) {
            D_800EAE88.random = 1;
        }
    }
    return 0;
}

int func_8002778C(DuelSelectionSource *source)
{
    int count = 0;
    int slot = DUEL_FIELD_ROW_SIZE;

    do {
        int position =
            slot + D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
        DuelCardRecord *entry = &D_801A7AD8[D_800907D8[position]];

        if (entry->flags & DUEL_CARD_FLAG_OCCUPIED) {
            count++;
            if (func_8001EFD4(DISPLAY_OBJECT_VIEW(source->ptr),
                              DISPLAY_OBJECT_VIEW(entry->object)) > 0)
                return ((DuelSelectionObject *)entry->object)->index;
        }
        slot++;
    } while (slot < DUEL_FIELD_SIDE_ZONE_COUNT);

    if (count == 0)
        return D_800907D8[
            D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT + 7
        ];
    return -1;
}

s32 func_800278A0(DuelSelectionSource *source)
{
    s32 i;
    s32 found = 0;
    DuelCardRecord *entry = 0;

    for (i = DUEL_FIELD_ROW_SIZE; i < DUEL_FIELD_SIDE_ZONE_COUNT; i++) {
        u8 row = D_800907D8[
            i + D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT
        ];
        entry = &D_801A7AD8[row];
        if (entry->flags & DUEL_CARD_FLAG_OCCUPIED) {
            found++;
            if (!(entry->flags & DUEL_CARD_FLAG_FACE_DOWN)) {
                s32 result = func_8001EFD4(
                    DISPLAY_OBJECT_VIEW(source->ptr),
                    DISPLAY_OBJECT_VIEW(entry->object));
                if (result > 0) {
                    return ((DuelSelectionObject *)entry->object)->index;
                }
            }
        }
    }

    if (found == 0) {
        return D_800907D8[
            D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT + 7
        ];
    }
    return -1;
}

/* The AI's per-turn play decision. Writes the chosen action into the three
 * trailing bytes of the D_800EAE88 selection: the action code in field_09,
 * its argument in field_0A, and a flag in field_0B. It does nothing while bit
 * 0x1000 of D_8009B16C is set.
 *
 * One turn in four it first looks for an equip: it collects the field's
 * monsters (type 0x17) and the hand's magic cards, and takes the first pair
 * Duel_CheckEquip accepts. Failing that it collects the type-0x14 monsters and
 * picks one at random. Otherwise, and when neither search finds anything, it
 * walks the five hand slots from 0xA to 0xE for the first card whose top two
 * flag bits read 10, and decides what to do with it through func_800278A0 and
 * func_8002778C, falling back to scanning the opponent's five monster zones
 * for one with 0x90000000 set.
 *
 * The walk body keeps retail's block order inside a do-while(0): the play
 * block first, then the func_8002778C fallback with the give-up block inline,
 * then the zone scan. Its record, grid and give-up arithmetic are written
 * index-first through integer casts, which is the operand order retail's
 * addu instructions show. */
s32 func_800279BC(void)
{
    DuelCardRecord *pool[6];
    DuelCardRecord *listb[6];
    DuelCardRecord *lista[6];
    DuelCardRecord **pa;
    DuelCardRecord **pb;
    DuelCardRecord **pp;
    DuelCardRecord *ea;
    DuelCardRecord *eb;
    DuelCardRecord *rec;
    DuelCardRecord *scan;
    s32 slot;
    s32 n;
    s32 i;
    s32 mask;
    s32 res;
    s32 count;
    s32 v;
    u8 *grid;
    AiSelection *out;
    DuelCardRecord *recs;
    DuelCardRecord *scanbase;
    s32 t;

    D_800EAE88.field_09 = 0;
    if (D_8009B16C & 0x1000) {
        return 0;
    }
    if ((rand() & 3) != 0) {
        slot = 0xA;
        goto walk;
    }
    {
        Duel_CollectFieldCardsByType((DuelCardRecord **)lista, 0xA, 0x17);
        Duel_CollectFieldCardsByType((DuelCardRecord **)listb, 5, -1);
        ea = lista[0];
        if (ea != 0) {
            pa = lista;
            do {
                eb = listb[0];
                if (eb != 0) {
                    pb = listb;
                    do {
                        if (Duel_CheckEquip((s16)ea->card_id,
                                            (s16)eb->card_id)) {
                            goto found;
                        }
                        pb++;
                        eb = *pb;
                    } while (eb != 0);
                }
                pa++;
                ea = *pa;
            } while (ea != 0);
        }
        n = Duel_CollectFieldCardsByType((DuelCardRecord **)lista, 0xA, 0x14);
        if (n == 0) {
            goto no_equip;
        }
        n--;
        if (n != 0) {
            n = Rand_GetInterval(n + 1);
        }
        D_800EAE88.field_09 = (s8)lista[n]->table_index % 5 + 6;
        D_800EAE88.field_0A = 0;
        D_800EAE88.field_0B = 0;
        return 0;
found:
        D_800EAE88.field_09 = (s8)ea->table_index % 5 + 6;
        D_800EAE88.field_0A = (s8)eb->table_index % 5 + 1;
        D_800EAE88.field_0B = 0;
        return 0;
no_equip:
        slot = 0xA;
    }
walk:
    grid = D_800907D8;
    out = &D_800EAE88;
    recs = D_801A7AD8;
    scanbase = &D_801A7AD8[5];
loop:
    {
        do {
            n = D_8009B1D5 * 20;
            rec = (DuelCardRecord *)((s32)*(u8 *)(slot + n + (s32)grid) * 28 + (s32)recs);
            /* Retail tests terrain_modifier and flags as one packed word. */
            if ((*(u32 *)&rec->terrain_modifier & 0xC0000000) == 0x80000000) {
                v = func_800278A0((DuelSelectionSource *)rec);
                if (v >= 0) {
    have:
                    if (D_8009B1C8->swords_turns_remaining == 0) {
                        t = (s8)rec->table_index;
                        slot = t % 5 + 1;
                        out->field_0B = 0;
                        out->field_0A = v % 5 + 0x38;
                        out->field_09 = slot;
                        return 0;
                    }
                    goto none;
                }
                if ((rand() & 3) == 0) {
                    v = func_8002778C((DuelSelectionSource *)rec);
                    if (v >= 0) {
                        goto have;
                    }
    none:
                    v = (s8)rec->table_index % 15;
                    out->field_0A = 0;
                    res = (s8)v - 4;
                    out->field_0B = 1;
                    out->field_09 = res;
                    return 0;
                }
                if ((rand() & 1) != 0) {
                    count = 0;
                    i = 0;
                    scan = (DuelCardRecord *)((s32)((D_8009B1D5 ^ 1) * 15) * 28 + (s32)scanbase);
                    mask = 0x90000000;
                    pp = pool;
                    do {
                        if ((*(u32 *)&scan->terrain_modifier & mask)
                            == mask) {
                            *pp = scan;
                            pp++;
                            count++;
                        }
                        i++;
                        scan++;
                    } while (i < 5);
                    if (count != 0) {
                        v = count - 1;
                        if (v != 0) {
                            v = Rand_GetInterval(count);
                        }
                        v = (s8)pool[v]->table_index;
                        goto have;
                    }
                }
                goto none;
            }
        } while (0);
        slot++;
    }
    if (slot < 0xF) {
        goto loop;
    }
    return 0;
}
