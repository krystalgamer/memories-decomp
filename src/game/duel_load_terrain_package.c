#define DUEL_TERRAIN_AS_ARRAY
#include "../types.h"
#include "duel_terrain_boost.h"
#include "duel_package.h"
#include "file_transfer.h"
#include "duel_load_package_stage.h"

/* Each terrain's package is DUEL_TERRAIN_LOAD_SECTOR_COUNT sectors long, and the
 * multiply is spelled as the shift sequence that matches. A regional build
 * defines all three. */
#ifndef DUEL_TERRAIN_LOAD_FIRST_SECTOR
#define DUEL_TERRAIN_LOAD_FIRST_SECTOR DUEL_TERRAIN_PACKAGE_FIRST_SECTOR
#endif

#ifndef DUEL_TERRAIN_LOAD_SECTOR_COUNT
#define DUEL_TERRAIN_LOAD_SECTOR_COUNT DUEL_TERRAIN_PACKAGE_SECTOR_COUNT
#endif

/* value * 0xEB */
#ifndef DUEL_TERRAIN_LOAD_OFFSET
#define DUEL_TERRAIN_LOAD_OFFSET(value) ((((value) * 15) * 4 - (value)) * 4 - (value))
#endif

void Duel_LoadTerrainPackage(void)
{
    int value = gDuel_bTerrain[0];
    int index = DUEL_TERRAIN_LOAD_OFFSET(value) + DUEL_TERRAIN_LOAD_FIRST_SECTOR;
    File_RequestAsyncTransfer(
        0, 0, index, DUEL_TERRAIN_LOAD_SECTOR_COUNT,
        Duel_LoadPackageStage, 0, 0);
    File_WaitForTransfers();
}
