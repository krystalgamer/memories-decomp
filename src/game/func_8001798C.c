#include "../types.h"
#include "duel_package.h"
#include "file_transfer.h"
#include "duel_load_package_stage.h"

/* One byte at 0x8009B364. This TU assembles at -G8, where a one-byte scalar
 * would be addressed %gp_rel; the array form is what keeps it out of small
 * data so the address is materialized absolutely. The size is irrelevant
 * here, unlike func_80024E58.c. See duel_terrain_boost.h. */
extern u8 gDuel_bTerrain[];

void func_8001798C(void)
{
    int value = gDuel_bTerrain[0];
    int index = (((value * 15) * 4 - value) * 4 - value) +
                DUEL_TERRAIN_PACKAGE_FIRST_SECTOR;
    File_RequestAsyncTransfer(
        0, 0, index, DUEL_TERRAIN_PACKAGE_SECTOR_COUNT,
        Duel_LoadPackageStage, 0, 0);
    File_WaitForTransfers();
}
