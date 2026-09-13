#ifndef MEMORIES_DECOMP_DUEL_REWARDS_H
#define MEMORIES_DECOMP_DUEL_REWARDS_H

#include "../types.h"
#include "card_constants.h"

/* One weighted card-drop row: CARD_COUNT halfword weights followed by the
 * established padding that makes each pool row 1,460 bytes. */
typedef struct {
    u16 weights[CARD_COUNT];
    u8 pad_05A4[DUEL_DROP_TABLE_PADDING_SIZE];
} DuelDropTable;

extern DuelDropTable gDuel_awSaPowCardDrops[];

void Duel_ShowResultPage(s32 page);
void Duel_CalcRankScore(void);
s32 Duel_SelectCardDrop(s32 pool_index);
void Duel_AwardCard(s32 card_id);

#endif
