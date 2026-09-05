#include "../types.h"
#include "card_constants.h"
#include "duel_card.h"

typedef struct LocalObj {
    u8 pad0[0x68];
    u8 f68;
} LocalObj;
typedef struct LocalEnt {
    u16 id;
    s16 x;
    s16 y;
    u16 flags;
    u8 b8;
    s8 b9;
    s8 bA;
    u8 bB;
} LocalEnt;
typedef struct LocalE9 {
    u8 pad0[0x18];
    s8 count;
    s8 pad19;
    s8 slots[6];
} LocalE9;
typedef struct LocalR6 {
    s16 id;
    u8 b2;
    u8 pad3[3];
} LocalR6;
typedef struct LocalBlob {
    u8 pad0[0x4B9FC];
    LocalR6 cards[1];
} LocalBlob;
extern LocalE9 D_800E9FF0[];
extern LocalBlob D_8015C424;
extern LocalR6 gDuel_aPlayerHand[];
extern s32 gDuel_adwCardStats[];
extern int Duel_CalcCardStats();

void func_80027DF8(LocalEnt *out, s32 who) {
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
            out->id = 0;
            if (rec->flags & 0x8000) {
                s32 r;
                s32 t;

                out->id = rec->card_id;
                r = Duel_CalcCardStats(rec);
                out->x = r;
                out->y = r >> 16;
                out->flags = rec->flags;
                out->b8 = ((LocalObj *)rec->object)->f68;
                if (rec->flags & 0x200) {
                    t = *(s32 *) ((((s16) rec->card_id - 1) << 2) + (u32) tbl) >>
                        CARD_STAT_GUARDIAN_STAR_2_SHIFT;
                } else {
                    t = *(s32 *) ((((s16) rec->card_id - 1) << 2) + (u32) tbl) >>
                        CARD_STAT_GUARDIAN_STAR_1_SHIFT;
                }
                out->b9 = t & CARD_STAT_GUARDIAN_STAR_MASK;
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
            out->id = 0;
            if (rec->flags & 0x8000) {
                s32 r;
                s32 t;

                out->id = rec->card_id;
                r = Duel_CalcCardStats(rec);
                out->x = r;
                out->y = r >> 16;
                out->flags = rec->flags;
                out->b8 = ((LocalObj *)rec->object)->f68;
                if (rec->flags & 0x200) {
                    t = *(s32 *) ((((s16) rec->card_id - 1) << 2) + (u32) tbl) >>
                        CARD_STAT_GUARDIAN_STAR_2_SHIFT;
                } else {
                    t = *(s32 *) ((((s16) rec->card_id - 1) << 2) + (u32) tbl) >>
                        CARD_STAT_GUARDIAN_STAR_1_SHIFT;
                }
                out->b9 = t & CARD_STAT_GUARDIAN_STAR_MASK;
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

            n = D_800E9FF0[who].slots[i];
            if (n >= 0) {
                s32 *p;
                s32 id;

                id = D_8015C424.cards[n].id;
                out->id = id;
                tbl = gDuel_adwCardStats;
                p = (s32 *) (((id - 1) << 2) + (u32) tbl);
                out->x = (*p & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                out->y = ((*p >> CARD_STAT_DEFENSE_SHIFT) &
                          CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                out->flags = 0;
                out->b8 =
                    (*p >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
                out->b9 = (*p >> CARD_STAT_GUARDIAN_STAR_1_SHIFT) &
                    CARD_STAT_GUARDIAN_STAR_MASK;
                out->bA = (*p >> CARD_STAT_GUARDIAN_STAR_2_SHIFT) &
                    CARD_STAT_GUARDIAN_STAR_MASK;
                out += 1;
            }
            i += 1;
        } while (i < HAND_SIZE);
    }

    {
        LocalR6 *rp;

        i = D_800E9FF0[who].count;
        rp = &gDuel_aPlayerHand[i + who * DECK_SIZE];
        if (i < DECK_SIZE) {
            s32 *tbl;

            tbl = gDuel_adwCardStats;
            do {
                s32 *p;
                s32 id;

                id = rp->id;
                out->id = id;
                p = (s32 *) (((id - 1) << 2) + (u32) tbl);
                out->x = (*p & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                out->y = ((*p >> CARD_STAT_DEFENSE_SHIFT) &
                          CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                out->flags = 0;
                out->b8 =
                    (*p >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
                i += 1;
                out->b9 = (*p >> CARD_STAT_GUARDIAN_STAR_1_SHIFT) &
                    CARD_STAT_GUARDIAN_STAR_MASK;
                out->bA = (*p >> CARD_STAT_GUARDIAN_STAR_2_SHIFT) &
                    CARD_STAT_GUARDIAN_STAR_MASK;
                out->bB = rp->b2;
                rp += 1;
                out += 1;
            } while (i < DECK_SIZE);
        }
    }
    out->id = 0;
}
