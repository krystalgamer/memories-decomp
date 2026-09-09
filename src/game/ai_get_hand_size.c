#include "../types.h"
#include "ai.h"
#include "ai_opponent_data.h"

extern s8 gDuel_bOpponentID;

s8 Ai_GetHandSize(void)
{
    return gDuel_aOpponentData[gDuel_bOpponentID].values[0];
}
