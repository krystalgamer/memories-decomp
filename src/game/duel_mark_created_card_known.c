#define D_8009B360_IN_DATA
#define GDUEL_BOPPONENTID_AS_SIZED_ARRAY
#include "../types.h"
#include "ai_opponent_data.h"
#include "duel_side_state.h"
#include "ai_controller_handoff.h"
#include "campaign_flags.h"
#include "duel_ritual_controller.h"

void Duel_MarkCreatedCardKnown(s32 card_id)
{
    if (gDuel_bActiveSide == 0 && gDuel_bSide0ControllerSelector < 0 && gDuel_bOpponentID[0] >= 0) {
        Library_UpdateCardUsedFlag((s32)((u32)card_id + CAMPAIGN_FLAG_LIBRARY_CARD_BASE));
    }
}
