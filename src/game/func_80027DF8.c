#include "../types.h"
#include "duel_side_state.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_card_display_state.h"
#include "duel_deck_card.h"
#include "duel_draw_resolution.h"
#include "func_80027DF8.h"

extern ExodiaCardDatabase D_8015C424;

void func_80027DF8(AiActiveCard *out, s32 who) {
    DuelCardRecord *base;
    s32 i;

    base = &D_801A7AD8[who * DUEL_CARD_SIDE_RECORD_COUNT];

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
                    ((DuelCardDisplayObject *)rec->object)->field_68;
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
                    ((DuelCardDisplayObject *)rec->object)->field_68;
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

            n = D_800E9FF0[who].hand[i];
            if (n >= 0) {
                s32 *p;
                s32 id;

                id = D_8015C424.cards[n].id;
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

        i = D_800E9FF0[who].field_18;
        rp = &gDuel_aDeckCardRecords[i + who * DECK_SIZE];
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
                out->deck_index = rp->index_02;
                rp += 1;
                out += 1;
            } while (i < DECK_SIZE);
        }
    }
    out->card_id = 0;
}
