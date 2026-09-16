#define DUEL_TERRAIN_AS_ARRAY
#include "../types.h"
#include "duel_terrain_boost.h"
#include "duel_package.h"
#include "file_transfer.h"
#include "duel_load_package_stage.h"

void Duel_LoadTerrainPackage(void)
{
    int value = gDuel_bTerrain[0];
    int index = (((value * 15) * 4 - value) * 4 - value) +
                DUEL_TERRAIN_PACKAGE_FIRST_SECTOR;
    File_RequestAsyncTransfer(
        0, 0, index, DUEL_TERRAIN_PACKAGE_SECTOR_COUNT,
        Duel_LoadPackageStage, 0, 0);
    File_WaitForTransfers();
}
