#ifndef MEMORIES_DECOMP_DUEL_TERRAIN_BOOST_H
#define MEMORIES_DECOMP_DUEL_TERRAIN_BOOST_H

#include "../types.h"
#include "card_constants.h"

/* Six terrains, matching the range Duel_GetTerrainBoost indexes with
   gDuel_bTerrain - 1. */
#define DUEL_TERRAIN_COUNT 6
#define DUEL_TERRAIN_STAT_ADJUSTMENT 500
#define DUEL_TERRAIN_BOOST_UNIT \
    (DUEL_TERRAIN_STAT_ADJUSTMENT / CARD_STAT_SCALE)
#define DUEL_TERRAIN_PENALTY_UNIT (-DUEL_TERRAIN_BOOST_UNIT)

/* gDuel_bTerrain is a single byte at 0x8009B364; gFreeDuel_bReturnFlags
   follows at 0x8009B365. Every user, at -G0 and -G8 alike, reaches it with an
   absolute %hi/%lo pair in the assembler macro form, which is what the .data
   placement gives: Duel_GetTerrainBoost materializes its address twice, and
   DuelEffect_ApplyTerrain and AiScript_LoadTerrain take the load at the
   assembler's expansion point. */
extern u8 gDuel_bTerrain;

/* Attack modifier in CARD_STAT_SCALE units, one row per monster card type and
   one column per terrain. Duel_GetTerrainBoost rejects cardType >=
   CARD_TYPE_MAGIC before indexing, so only the monster types have rows.
   src/game/duel_scene.c owns the table at 0x800909D4. */
extern s8 gDuel_aTerrainBoost[CARD_TYPE_MAGIC][DUEL_TERRAIN_COUNT];

/* Returns the attack/defence adjustment the active terrain gives a card of
   `cardType`, already multiplied by CARD_STAT_SCALE, or 0 when there is no
   terrain or the card is not a monster. */
s32 Duel_GetTerrainBoost(s32 cardType);

#endif
