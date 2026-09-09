#include "../types.h"
#include "data_transfer_request.h"
#include "mem_card.h"
#include "save_data.h"
#include "util_memory.h"
void SaveData_RequestWrite(void){Util_CopyWords(gSaveData_aTransferBuffer,(u8 *)gDuel_awPlayerDeck,SAVE_DATA_STATE_SIZE);SaveData_BuildPayload(gSaveData_aTransferBuffer-SAVE_DATA_HEADER_SIZE);func_8003F758(gSaveData_aTransferBuffer,SAVE_DATA_REPLICATED_STATE_SIZE,gMemCard_szSaveFileName,2);}
