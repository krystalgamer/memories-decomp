#include "../../types.h"

#define VERSION_JAPAN
#define VERSION_JAPAN_SAVE_DATA_REQUEST_LOAD
#define gSaveData_aTransferBuffer D_801D3200
#define gMemCard_szSaveFileName D_80010300
#define SAVE_DATA_LOAD_STATUS_ADDRESS 0x8009AFC1
#include "../save_data_transfer_runtime.c"
