#include "../../types.h"

/* SLPM-86398 build of src/game/duel_load_terrain_package.c. The Japanese WA.MRG
 * gives each terrain package 0xEF sectors from 0x16B5 (US: 0xEB from 0x16C6), and
 * the multiply by 0xEF is one shift pair shorter. */
#define gDuel_bTerrain gJapanese_DuelTerrain

#define DUEL_TERRAIN_LOAD_FIRST_SECTOR 0x16B5
#define DUEL_TERRAIN_LOAD_SECTOR_COUNT 0xEF
/* value * 0xEF */
#define DUEL_TERRAIN_LOAD_OFFSET(value) (((value) * 15) * 16 - (value))

#include "../duel_load_terrain_package.c"
