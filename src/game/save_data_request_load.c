#include "../types.h"
#include "data_transfer_request.h"
#include "mem_card.h"
#include "save_data.h"

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
