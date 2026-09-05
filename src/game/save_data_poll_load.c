#include "../types.h"
#include "save_data.h"

extern s32 func_8003F70C(void);
extern void Util_CopyWords(void *, void *, s32);
extern void SaveData_ApplyRuntimeState(void *);
extern u8 gDuel_awPlayerDeck[], D_801D3200[];
s32 SaveData_PollLoad(void) {
    s32 r = func_8003F70C();
    if (r != 0) {
        if (r == 1) {
            u8 *p = gDuel_awPlayerDeck;
            Util_CopyWords(p, D_801D3200, SAVE_DATA_STATE_SIZE);
            SaveData_ApplyRuntimeState(p);
        }
        return r;
    }
    return 0;
}
