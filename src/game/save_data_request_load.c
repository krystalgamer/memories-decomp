#include "../types.h"
#include "mem_card.h"
#include "save_data.h"

extern void func_8003F758(u8 *, s32, u8 *, s32);

void SaveData_RequestLoad(void)
{
    /* The symbolic store changes the target $at scheduling and relocation. */
    *(u8 *)0x8009B0D1 = 0;
    func_8003F758(
        gSaveData_aTransferBuffer,
        SAVE_DATA_STATE_SIZE,
        gMemCard_szSaveFileName,
        0
    );
}
