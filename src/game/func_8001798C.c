#include "../types.h"
#include "duel_package.h"
#include "file_transfer.h"

extern u8 gDuel_bTerrain[9];
extern void Duel_LoadPackageStage(void);
extern void func_800137E4(void);

void func_8001798C(void)
{
    int value = gDuel_bTerrain[0];
    int index = (((value * 15) * 4 - value) * 4 - value) +
                DUEL_TERRAIN_PACKAGE_FIRST_SECTOR;
    File_RequestAsyncTransfer(
        0, 0, index, DUEL_TERRAIN_PACKAGE_SECTOR_COUNT,
        Duel_LoadPackageStage, 0, 0);
    func_800137E4();
}
