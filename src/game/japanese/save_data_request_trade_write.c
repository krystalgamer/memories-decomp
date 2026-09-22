#include "../../types.h"

#define VERSION_JAPAN
#define VERSION_JAPAN_SAVE_DATA_REQUEST_TRADE_WRITE
#define D_801D1880 gJapanese_TradeSaveBuffer
#define SaveData_WritePrimarySecondaryIntegrity func_8003C4D8
#define gMemCard_pSecondaryTransferCursor gJapanese_SecondaryTransferCursor
#define MemCardDialog_Request func_8003EE2C
#define gMemCard_szSaveFileName D_80010300
#include "../save_data_transfer_runtime.c"
