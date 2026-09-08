#include "../types.h"
#include "save_data.h"

extern s32 func_8003F70C(void);
extern void Util_CopyWords(void *, void *, s32);
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
