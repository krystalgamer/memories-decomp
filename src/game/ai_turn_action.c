#define DISPLAY_OBJECT_VIEW(object) ((DisplayObject *)(object))
#define DUEL_CARD_STAGING_DECK_VIEW
#include "../types.h"
#include "../psyq/rand.h"
#include "ai.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_grid.h"
#include "duel_side_state.h"
#include "rand_get_interval.h"
#include "duel_field_effects.h"
#include "duel_card_checks.h"
#include "duel_check_quit_input.h"
#include "ai_turn_action.h"
#include "duel_battle_stats.h"
#include "../unmatched.h"
#include "duel_card_staging.h"
#include "duel_card_display_state.h"
#include "duel_deck_card.h"
#include "ai_board_snapshot.h"

/* The AI's turn-action run: spell and fusion searches feed the action pick
   through gAi_Selection, followed by the occupied and face-up field-target
   selectors and the final per-turn play decision that consumes them. All six
   functions are contiguous and share the gcc_2_8_1_g8_split profile. */

/* The pending selection lives in ai.h. Keep its matched field accesses and
   the independent interior-symbol aliases used by the script handlers. */

/* Signed locals preserve the spell search's table_index reads from the
   canonical records, whose stored byte is unsigned. */

int Ai_SelectMagicPlay(void)
{
 DuelCardRecord*list[6];int count=Duel_CollectFieldCardsByType(list,0,CARD_TYPE_MAGIC);
 if(count){s8 first=list[0]->table_index;int index;gAi_Selection.fields.result=first%DUEL_FIELD_ROW_SIZE+11;gAi_Selection.fields.field1=0;gAi_Selection.fields.zero=0;gAi_Selection.fields.random=0;gAi_Selection.fields.value=6;
  index=Duel_FindFreeFieldSlot(DUEL_FIELD_SIDE_ZONE_COUNT);if(index>=0){gAi_Selection.fields.random=rand()&1;gAi_Selection.fields.value+=index%DUEL_FIELD_ROW_SIZE;}return 0;}return 1;
}

/* AI fusion attempt. Collects hand monsters, rolls a coin, and looks for a
   fusable pair with the occupied monster row (first pass) or, when a field
   slot is free, between two distinct hand entries (second pass).
   A hit fills the gAi_Selection selection with the two slot
   bytes and the target slot; returns 0 on a selection, 1 otherwise. */
s32 Ai_SelectFusionPlay(void) {
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
    AiSelectionFields *st;
    s8 v;

    st = &gAi_Selection.fields;
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
    AiSelectionFields *st;
    s8 v;
    s8 w;

    st = &gAi_Selection.fields;
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
   odds, then picks a random card from the side's hand records and fills
   the gAi_Selection selection with a field slot for it: a monster goes to the
   front row with a random flag, anything else to the back row, forcing the
   flag on for an equip. Always returns 0. */
s32 Ai_SelectHandPlay(void) {
    DuelCardRecord *card;
    s32 slot;

    if ((rand() & 1) == 0 && Duel_SelectEquipPair() == 0) {
        return 0;
    }
    if ((rand() & 1) == 0 && Duel_SelectTrapPlay() == 0) {
        return 0;
    }
    if ((rand() & 3) == 0 && Ai_SelectMagicPlay() == 0) {
        return 0;
    }
    if (Ai_SelectFusionPlay() == 0) {
        return 0;
    }
    card = &gDuel_aCardRecords[
        gDuel_bActiveSide * DUEL_CARD_SIDE_RECORD_COUNT +
        Rand_GetInterval(DUEL_FIELD_ROW_SIZE)
    ];
    if (((gDuel_adwCardStats[*(s16 *)&card->card_id - 1] >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK) < CARD_TYPE_MAGIC) {
        s8 v;

        slot = Duel_FindFreeFieldSlot(DUEL_FIELD_ROW_SIZE);
        if (slot < 0) {
            slot = Rand_GetInterval(DUEL_FIELD_ROW_SIZE);
        }
        v = card->table_index;
        gAi_Selection.fields.field1 = 0;
        gAi_Selection.fields.value = slot % DUEL_FIELD_ROW_SIZE + 1;
        gAi_Selection.fields.result = v % DUEL_FIELD_ROW_SIZE + 0xB;
        gAi_Selection.fields.zero = rand() & 1;
        gAi_Selection.fields.random = 1;
    } else {
        s8 v;

        slot = Duel_FindFreeFieldSlot(DUEL_FIELD_SIDE_ZONE_COUNT);
        if (slot < 0) {
            slot = Rand_GetInterval(DUEL_FIELD_ROW_SIZE);
        }
        v = card->table_index;
        gAi_Selection.fields.field1 = 0;
        gAi_Selection.fields.zero = 0;
        gAi_Selection.fields.value = slot % DUEL_FIELD_ROW_SIZE + 6;
        gAi_Selection.fields.result = v % DUEL_FIELD_ROW_SIZE + 0xB;
        gAi_Selection.fields.random = rand() & 1;
        if (((gDuel_adwCardStats[*(s16 *)&card->card_id - 1] >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK) == CARD_TYPE_EQUIP) {
            gAi_Selection.fields.random = 1;
        }
    }
    return 0;
}

int Ai_FindFavorableFieldTarget(DuelCardRecord *source)
{
    int count = 0;
    int slot = DUEL_FIELD_ROW_SIZE;

    do {
        int position =
            slot + gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
        DuelCardRecord *entry = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices[position]];

        if (entry->flags & DUEL_CARD_FLAG_OCCUPIED) {
            count++;
            if (Duel_CompareBattleStrength(DISPLAY_OBJECT_VIEW(source->object),
                              DISPLAY_OBJECT_VIEW(entry->object)) > 0)
                return DISPLAY_OBJECT_VIEW(entry->object)->field_6A;
        }
        slot++;
    } while (slot < DUEL_FIELD_SIDE_ZONE_COUNT);

    if (count == 0)
        return gDuel_abFieldGridCardRecordIndices[
            gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT + 7
        ];
    return -1;
}

s32 Ai_FindFavorableFaceUpFieldTarget(DuelCardRecord *source)
{
    s32 i;
    s32 found = 0;
    DuelCardRecord *entry = 0;

    for (i = DUEL_FIELD_ROW_SIZE; i < DUEL_FIELD_SIDE_ZONE_COUNT; i++) {
        u8 row = gDuel_abFieldGridCardRecordIndices[
            i + gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT
        ];
        entry = &gDuel_aCardRecords[row];
        if (entry->flags & DUEL_CARD_FLAG_OCCUPIED) {
            found++;
            if (!(entry->flags & DUEL_CARD_FLAG_FACE_DOWN)) {
                s32 result = Duel_CompareBattleStrength(
                    DISPLAY_OBJECT_VIEW(source->object),
                    DISPLAY_OBJECT_VIEW(entry->object));
                if (result > 0) {
                    return DISPLAY_OBJECT_VIEW(entry->object)->field_6A;
                }
            }
        }
    }

    if (found == 0) {
        return gDuel_abFieldGridCardRecordIndices[
            gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT + 7
        ];
    }
    return -1;
}

/* The AI's per-turn play decision. Writes the chosen action into the three
 * trailing bytes of the gAi_Selection selection: the action code in field_09,
 * its argument in field_0A, and a flag in field_0B. It does nothing while bit
 * 0x1000 of gDuel_wOutcomeFlags is set.
 *
 * One turn in four it first looks for an equip: it collects the field's
 * equips (type 0x17) and occupied monster-row cards, and takes the first pair
 * Duel_CheckEquip accepts. Failing that it collects the type-0x14 magic cards and
 * picks one at random. Otherwise, and when neither search finds anything, it
 * walks grid slots 0xA to 0xE for the first card whose top two
 * flag bits read 10, and decides what to do with it through Ai_FindFavorableFaceUpFieldTarget and
 * Ai_FindFavorableFieldTarget, falling back to scanning the opponent's five monster zones
 * for one with 0x90000000 set.
 *
 * The walk body keeps retail's block order inside a do-while(0): the play
 * block first, then the Ai_FindFavorableFieldTarget fallback with the give-up block inline,
 * then the zone scan. Its record, grid and give-up arithmetic are written
 * index-first through integer casts, which is the operand order retail's
 * addu instructions show. */
s32 Ai_SelectFieldPlay(void)
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
    AiSelectionFields *out;
    DuelCardRecord *recs;
    DuelCardRecord *scanbase;
    s32 t;

    gAi_Selection.fields.field_09 = 0;
    if (gDuel_wOutcomeFlags & 0x1000) {
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
        gAi_Selection.fields.field_09 = (s8)lista[n]->table_index % 5 + 6;
        gAi_Selection.fields.field_0A = 0;
        gAi_Selection.fields.field_0B = 0;
        return 0;
found:
        gAi_Selection.fields.field_09 = (s8)ea->table_index % 5 + 6;
        gAi_Selection.fields.field_0A = (s8)eb->table_index % 5 + 1;
        gAi_Selection.fields.field_0B = 0;
        return 0;
no_equip:
        slot = 0xA;
    }
walk:
    grid = gDuel_abFieldGridCardRecordIndices;
    out = &gAi_Selection.fields;
    recs = gDuel_aCardRecords;
    scanbase = &gDuel_aCardRecords[5];
loop:
    {
        do {
            n = gDuel_bActiveSide * 20;
            rec = (DuelCardRecord *)((s32)*(u8 *)(slot + n + (s32)grid) * 28 + (s32)recs);
            /* Retail tests terrain_modifier and flags as one packed word. */
            if ((*(u32 *)&rec->terrain_modifier & 0xC0000000) == 0x80000000) {
                v = Ai_FindFavorableFaceUpFieldTarget(rec);
                if (v >= 0) {
    have:
                    if (gDuel_pActiveSideState->swords_turns_remaining == 0) {
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
                    v = Ai_FindFavorableFieldTarget(rec);
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
                    scan = (DuelCardRecord *)((s32)((gDuel_bActiveSide ^ 1) * 15) * 28 + (s32)scanbase);
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

void Ai_ExportSideSnapshot(AiActiveCard *out, s32 side) {
    DuelCardRecord *base;
    s32 i;

    base = &gDuel_aCardRecords[side * DUEL_CARD_SIDE_RECORD_COUNT];

    {
        DuelCardRecord *rec;
        s32 *tbl;

        rec = base + DUEL_FIELD_ROW_SIZE;
        i = 0;
        tbl = gDuel_adwCardStats;
        do {
            out->card_id = 0;
            if (rec->flags & DUEL_CARD_FLAG_OCCUPIED) {
                s32 r;
                s32 t;

                out->card_id = rec->card_id;
                r = Duel_CalcCardStats(rec);
                out->attack = r;
                out->defense = r >> 16;
                out->flags = rec->flags;
                out->card_type =
                    ((DuelCardDisplayObject *)rec->object)->card_type;
                if (rec->flags & DUEL_CARD_FLAG_USE_GUARDIAN_STAR_2) {
                    t = *(s32 *) ((((s16) rec->card_id - 1) << 2) + (u32) tbl) >>
                        CARD_STAT_GUARDIAN_STAR_2_SHIFT;
                } else {
                    t = *(s32 *) ((((s16) rec->card_id - 1) << 2) + (u32) tbl) >>
                        CARD_STAT_GUARDIAN_STAR_1_SHIFT;
                }
                out->guardian_star = t & CARD_STAT_GUARDIAN_STAR_MASK;
            }
            i += 1;
            out += 1;
            rec += 1;
        } while (i < DUEL_FIELD_ROW_SIZE);
    }

    {
        DuelCardRecord *rec;
        s32 *tbl;

        rec = base + DUEL_FIELD_SIDE_ZONE_COUNT;
        i = 0;
        tbl = gDuel_adwCardStats;
        do {
            out->card_id = 0;
            if (rec->flags & DUEL_CARD_FLAG_OCCUPIED) {
                s32 r;
                s32 t;

                out->card_id = rec->card_id;
                r = Duel_CalcCardStats(rec);
                out->attack = r;
                out->defense = r >> 16;
                out->flags = rec->flags;
                out->card_type =
                    ((DuelCardDisplayObject *)rec->object)->card_type;
                if (rec->flags & DUEL_CARD_FLAG_USE_GUARDIAN_STAR_2) {
                    t = *(s32 *) ((((s16) rec->card_id - 1) << 2) + (u32) tbl) >>
                        CARD_STAT_GUARDIAN_STAR_2_SHIFT;
                } else {
                    t = *(s32 *) ((((s16) rec->card_id - 1) << 2) + (u32) tbl) >>
                        CARD_STAT_GUARDIAN_STAR_1_SHIFT;
                }
                out->guardian_star = t & CARD_STAT_GUARDIAN_STAR_MASK;
            }
            i += 1;
            out += 1;
            rec += 1;
        } while (i < DUEL_FIELD_ROW_SIZE);
    }

    {
        s32 *tbl;

        i = 0;
        do {
            s32 n;

            n = gDuel_aSideStates[side].hand[i];
            if (n >= 0) {
                s32 *p;
                s32 id;

                id = D_8015C424_cards.cards[n].id;
                out->card_id = id;
                tbl = gDuel_adwCardStats;
                p = (s32 *) (((id - 1) << 2) + (u32) tbl);
                out->attack = (*p & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                out->defense = ((*p >> CARD_STAT_DEFENSE_SHIFT) &
                                CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                out->flags = 0;
                out->card_type =
                    (*p >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
                out->guardian_star =
                    (*p >> CARD_STAT_GUARDIAN_STAR_1_SHIFT) &
                    CARD_STAT_GUARDIAN_STAR_MASK;
                out->guardian_star_2 =
                    (*p >> CARD_STAT_GUARDIAN_STAR_2_SHIFT) &
                    CARD_STAT_GUARDIAN_STAR_MASK;
                out += 1;
            }
            i += 1;
        } while (i < HAND_SIZE);
    }

    {
        DuelDeckCardRecord *rp;

        i = gDuel_aSideStates[side].deck_draw_cursor;
        rp = &gDuel_aDeckCardRecords[i + side * DECK_SIZE];
        if (i < DECK_SIZE) {
            s32 *tbl;

            tbl = gDuel_adwCardStats;
            do {
                s32 *p;
                s32 id;

                id = rp->id;
                out->card_id = id;
                p = (s32 *) (((id - 1) << 2) + (u32) tbl);
                out->attack = (*p & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                out->defense = ((*p >> CARD_STAT_DEFENSE_SHIFT) &
                                CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                out->flags = 0;
                out->card_type =
                    (*p >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
                i += 1;
                out->guardian_star =
                    (*p >> CARD_STAT_GUARDIAN_STAR_1_SHIFT) &
                    CARD_STAT_GUARDIAN_STAR_MASK;
                out->guardian_star_2 =
                    (*p >> CARD_STAT_GUARDIAN_STAR_2_SHIFT) &
                    CARD_STAT_GUARDIAN_STAR_MASK;
                out->deck_index = rp->deck_index;
                rp += 1;
                out += 1;
            } while (i < DECK_SIZE);
        }
    }
    out->card_id = 0;
}
