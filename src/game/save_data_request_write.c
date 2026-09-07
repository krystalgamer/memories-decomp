#include "../types.h"
#include "save_data.h"

extern u8 D_801D3200[],gDuel_awPlayerDeck[],D_80010384[];extern void Util_CopyWords(void*,void*,u32);extern void func_8003F758(void*,s32,void*,s32);
void SaveData_RequestWrite(void){Util_CopyWords(D_801D3200,gDuel_awPlayerDeck,SAVE_DATA_STATE_SIZE);SaveData_BuildPayload(D_801D3200-SAVE_DATA_HEADER_SIZE);func_8003F758(D_801D3200,SAVE_DATA_REPLICATED_STATE_SIZE,D_80010384,2);}
