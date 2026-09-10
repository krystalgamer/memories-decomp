#include "../types.h"
#include "mem_card.h"
#include "save_data_update_load_pair.h"
#include "save_data_update_trade_load.h"

s32 SaveData_UpdateTradeLoad(void)
{
    if ((D_8009B3ED & 0x80) == 0) {
        D_8009B3ED |= 0x80;
        D_8009B3C0 = 0x29;
    }
    return SaveData_UpdateLoadPair();
}
