#include "../types.h"
#include "card_constants.h"

typedef struct {
    u16 weights[CARD_COUNT];
    u8 pad05A4[DUEL_DROP_TABLE_PADDING_SIZE];
} DuelDropTable;

extern DuelDropTable gDuel_awSaPowCardDrops[];
extern u8 gDuel_awPlayerDeck[];

extern s32 rand(void);

s32 Duel_SelectCardDrop(s32 opponent)
{
    DuelDropTable *table = &gDuel_awSaPowCardDrops[opponent];
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
    u8 *base = gDuel_awPlayerDeck;
    u8 *quantity = base + (card_id + 0x4F);
    u16 *destination = (u16 *)(base + 0x5BC);
    u16 *entry;

    (*quantity)++;
    if (*quantity >= 0xFB)
        *quantity = 0xFA;
    i = DUEL_RECENT_CARD_DROP_COUNT - 2;
    entry = destination + (DUEL_RECENT_CARD_DROP_COUNT - 2);
    do {
        entry[1] = entry[0];
        entry--;
        i--;
    } while (i >= 0);
    *destination = card_id;
}
