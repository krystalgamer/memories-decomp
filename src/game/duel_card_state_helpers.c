#include "../types.h"
#include "duel_side_state.h"
#include "duel_card_layout.h"
#include "ai_board_snapshot.h"

void Ai_RefreshBoardSnapshot(void)
{
    Ai_ExportSideSnapshot(gAiSnapshot_aOwnCards, gDuel_bActiveSide);
    Ai_ExportSideSnapshot(gAiSnapshot_aOwnCards + AI_ACTIVE_CARD_SIDE_SLOT_STRIDE,
                 gDuel_bActiveSide ^ 1);
}

int DuelCard_DecodeTaggedSlotIndex(int value)
{
    if (value & 0x80) {
        return (value & 0x7F) + DUEL_CARD_SIDE_RECORD_COUNT;
    }
    return value;
}
