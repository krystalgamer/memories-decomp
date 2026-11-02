#include "../../types.h"

#define VERSION_JAPAN
#define DUEL_INIT_TERRAIN_SECTOR(value) ((((value) * 15) * 16 - (value)) + 0x16B5)
#define DUEL_INIT_TERRAIN_SECTOR_COUNT 0xEF
#define DUEL_INIT_DUELIST_DATA_FIRST_SECTOR 0x1D3E
#include "../duel_init_scene.c"
