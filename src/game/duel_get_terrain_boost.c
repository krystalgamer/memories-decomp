#include "../types.h"
#include "card_constants.h"
#include "duel_terrain_boost.h"

extern u8 gDuel_bTerrain[];
/* Same byte, distinct compiler identity: keep both address materializations. */
extern u8 gDuel_bTerrainCodegenAlias[];

s32 Duel_GetTerrainBoost(s32 cardType)
{
    u8 *terrain = gDuel_bTerrainCodegenAlias;

    if (gDuel_bTerrain[0] == 0 || cardType >= CARD_TYPE_MAGIC) {
        return 0;
    }

    return gDuel_aTerrainBoost[cardType][terrain[0] - 1] * CARD_STAT_SCALE;
}
