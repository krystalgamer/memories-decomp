#include "../types.h"
#include "save_data.h"

u32 gSaveData_dwMaskStateLow __attribute__((section(".sdata"))) = 0x55555555;
u32 gSaveData_dwMaskStateHigh __attribute__((section(".sdata"))) = 0x55555555;
