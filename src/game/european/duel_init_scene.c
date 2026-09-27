#include "../../types.h"

#define VERSION_EUROPE
#define VERSION_EUROPE_DUEL_INIT_SCENE
#define DUEL_INIT_TERRAIN_SECTOR(value) ((((value) * 15) * 16) + 0x1B88)
#define DUEL_INIT_TERRAIN_SECTOR_COUNT 0xF0
#define DUEL_INIT_DUELIST_DATA_FIRST_SECTOR 0x2218
#define DUEL_INIT_RESOURCE_FIELD_2C 0x280
#define DUEL_INIT_RESOURCE_FIELD_2E 0xDF
#define D_8009B1DC gEuropean_D_8009B1DC
#define Duel_LoadPackageStage func_800170C0

#include "../duel_init_scene.c"
