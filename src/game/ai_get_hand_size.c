#include "../types.h"
#include "ai.h"
#include "ai_opponent_data.h"

s8 Ai_GetHandSize(void)
{
    return gDuel_aOpponentData[gDuel_bOpponentID].values[0];
}
