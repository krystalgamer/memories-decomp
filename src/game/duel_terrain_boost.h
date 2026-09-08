#ifndef MEMORIES_DECOMP_DUEL_TERRAIN_BOOST_H
#define MEMORIES_DECOMP_DUEL_TERRAIN_BOOST_H

#include "../types.h"
#include "card_constants.h"

/* Six terrains, matching the range Duel_GetTerrainBoost indexes with
   gDuel_bTerrain[0] - 1. */
#define DUEL_TERRAIN_COUNT 6

/* Attack modifier as a percentage step, one row per monster card type and one
   column per terrain. Duel_GetTerrainBoost rejects cardType >= CARD_TYPE_MAGIC
   before indexing, so only the monster types have rows.
   src/game/duel_terrain_boost.c owns the table at 0x800909D4. */
extern s8 gDuel_aTerrainBoost[CARD_TYPE_MAGIC][DUEL_TERRAIN_COUNT];

#endif
