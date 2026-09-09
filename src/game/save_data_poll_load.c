#include "../types.h"
#include "data_transfer_request.h"
#include "save_data.h"
#include "util_memory.h"
extern void SaveData_ApplyRuntimeState(void *);
s32 SaveData_PollLoad(void) {
    s32 r = func_8003F70C();
    if (r != 0) {
        if (r == 1) {
            u8 *p = (u8 *)gDuel_awPlayerDeck;
            Util_CopyWords(p, gSaveData_aTransferBuffer, SAVE_DATA_STATE_SIZE);
            SaveData_ApplyRuntimeState(p);
        }
        return r;
    }
    return 0;
}
