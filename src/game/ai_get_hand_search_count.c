#include "../types.h"
#include "ai.h"
#include "ai_opponent_data.h"

s8 Ai_GetHandSearchCount(void)
{
    return gDuel_aOpponentData[gDuel_bOpponentID].values[
        AI_OPPONENT_HAND_SEARCH_COUNT_FIELD];
}
