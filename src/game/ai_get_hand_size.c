#include "../types.h"
#include "ai.h"
#define AI_OPPONENT_ID_IN_DATA
#include "ai_opponent_data.h"

s8 Ai_GetHandSize(void)
{
    return gDuel_aOpponentData[gDuel_bOpponentID].values[0];
}
