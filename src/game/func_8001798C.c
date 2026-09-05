#include "../types.h"

extern u8 gDuel_bTerrain[9];
extern void Duel_LoadPackageStage(void);
extern void File_RequestAsyncTransfer();
extern void func_800137E4(void);

void func_8001798C(void)
{
    int value = gDuel_bTerrain[0];
    int index = (((value * 15) * 4 - value) * 4 - value) + 0x16C6;
    File_RequestAsyncTransfer(0, 0, index, 0xEB, Duel_LoadPackageStage, 0, 0);
    func_800137E4();
}
