#include "../../types.h"

#define VERSION_JAPAN
#define VERSION_JAPAN_DUEL_GET_TERRAIN_BOOST
#define gDuel_bTerrain gJapanese_DuelTerrain
extern u8 gDuel_bTerrainCodegenAlias[] asm("gJapanese_DuelTerrain");
#include "../duel_card_record_lifecycle.c"
