#include "../types.h"
#include "card_constants.h"

extern u8 gDuel_bTerrain[];
extern u8 D_8009B364[];
extern s8 gDuel_aTerrainBoost[][6];

s32 Duel_GetTerrainBoost(s32 card)
{
    u8 *terrain = D_8009B364;

    if (gDuel_bTerrain[0] == 0 || card >= 20) {
        return 0;
    }

    return gDuel_aTerrainBoost[card][terrain[0] - 1] * CARD_STAT_SCALE;
}
