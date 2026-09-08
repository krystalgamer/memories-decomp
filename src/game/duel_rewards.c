#include "../types.h"
#include "save_data.h"
#include "duel_side_state.h"
#include "../psyq/rand.h"
#include "card_constants.h"
#include "duel_grid.h"
#include "duel_rank.h"

extern u8 *D_8009B1E8;
extern u8 gDuel_bWinnerSide;
extern s32 D_801D5608[16][DUEL_SIDE_COUNT];

s32 Duel_CalcRankScoreChange(s32, s32);

/* Initializes result-message selectors at +0x34; the winner's signed
   end-reason adjustment selects the middle variant, not a rank letter.
   Both side scores at +0x2C start at DUEL_RANK_SCORE_INITIAL, then receive
   the end-reason and threshold-rule adjustments. Raw statistics are also
   copied into the separate D_801D5608[stat][side] display table. */
void Duel_CalcRankScore(void) {
    u8 *p;
    DuelSideState *e;
    s32 i;
    s32 v;
    s32 *q;

    p = D_8009B1E8;
    e = D_800E9FF0;
    q = &D_801D5608[0][0];
    p[0x34] = 0x44;
    p[0x35] = 0x40;
    p[0x36] = 0x45;
    if (D_800E9FF0[gDuel_bWinnerSide].field_00 == DUEL_RANK_ADJUST_EXODIA_WIN) {
        p[0x35] = 0x42;
    }
    if (D_800E9FF0[gDuel_bWinnerSide].field_00 == DUEL_RANK_ADJUST_DECK_OUT_WIN) {
        p[0x35] = 0x41;
    }

    *(s32 *)(p + 0x30) = DUEL_RANK_SCORE_INITIAL;
    *(s32 *)(p + 0x2C) = DUEL_RANK_SCORE_INITIAL;
    for (i = 0; i < DUEL_SIDE_COUNT; i++, e++, q++) {
        *(s32 *)(p + 0x2C + i * 4) += e->field_00;
        v = e->field_18; q[0 * DUEL_SIDE_COUNT] = v;
        *(s32 *)(p + 0x2C + i * 4) +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_CARDS_USED, v);
        v = e->life_points.signed_value; q[1 * DUEL_SIDE_COUNT] = v;
        *(s32 *)(p + 0x2C + i * 4) +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_REMAINING_LP, v);
        q[2 * DUEL_SIDE_COUNT] = e->field_0E;
        q[3 * DUEL_SIDE_COUNT] = e->field_0B;
        v = e->field_02; q[4 * DUEL_SIDE_COUNT] = v;
        *(s32 *)(p + 0x2C + i * 4) +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_EFFECTIVE_ATTACKS, v);
        q[5 * DUEL_SIDE_COUNT] = e->field_10;
        q[6 * DUEL_SIDE_COUNT] = e->field_0C;
        v = e->field_03; q[7 * DUEL_SIDE_COUNT] = v;
        *(s32 *)(p + 0x2C + i * 4) +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_DEFENSIVE_WINS, v);
        q[8 * DUEL_SIDE_COUNT] = e->field_07;
        v = e->field_04; q[9 * DUEL_SIDE_COUNT] = v;
        *(s32 *)(p + 0x2C + i * 4) +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_FACE_DOWN_PLAYS, v);
        v = e->field_08; q[10 * DUEL_SIDE_COUNT] = v;
        *(s32 *)(p + 0x2C + i * 4) +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_INITIATE_FUSION, v);
        v = e->field_09; q[11 * DUEL_SIDE_COUNT] = v;
        *(s32 *)(p + 0x2C + i * 4) +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_EQUIP_MAGIC, v);
        q[12 * DUEL_SIDE_COUNT] = e->field_0A;
        v = e->field_05; q[13 * DUEL_SIDE_COUNT] = v;
        *(s32 *)(p + 0x2C + i * 4) +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_PURE_MAGIC, v);
        v = e->field_06; q[14 * DUEL_SIDE_COUNT] = v;
        *(s32 *)(p + 0x2C + i * 4) +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_TRAPS_TRIGGERED, v);
        v = e->field_01; q[15 * DUEL_SIDE_COUNT] = v;
        *(s32 *)(p + 0x2C + i * 4) +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_TURNS, v);
    }
}

typedef struct {
    u16 weights[CARD_COUNT];
    u8 pad05A4[DUEL_DROP_TABLE_PADDING_SIZE];
} DuelDropTable;

extern DuelDropTable gDuel_awSaPowCardDrops[];

s32 Duel_SelectCardDrop(s32 pool_index)
{
    DuelDropTable *table = &gDuel_awSaPowCardDrops[pool_index];
    s32 threshold = (rand() & (DUEL_DROP_WEIGHT_TOTAL - 1)) + 1;
    s32 sum = 0;
    s32 i;

    for (i = 0; i < CARD_COUNT; i++) {
        sum += table->weights[i];
        if (sum >= threshold)
            return i + 1;
    }
    return 0;
}

void Duel_AwardCard(s32 card_id)
{
    s32 i;
    u8 *base = (u8 *)gDuel_awPlayerDeck;
    u8 *quantity = base + (card_id + 0x4F);
    u16 *destination = (u16 *)(base + 0x5BC);
    u16 *entry;

    (*quantity)++;
    if (*quantity >= CARD_CHEST_QUANTITY_MAX + 1)
        *quantity = CARD_CHEST_QUANTITY_MAX;
    i = DUEL_RECENT_CARD_DROP_COUNT - 2;
    entry = destination + (DUEL_RECENT_CARD_DROP_COUNT - 2);
    do {
        entry[1] = entry[0];
        entry--;
        i--;
    } while (i >= 0);
    *destination = card_id;
}
