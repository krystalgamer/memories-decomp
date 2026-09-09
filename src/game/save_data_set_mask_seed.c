#include "../types.h"
#include "save_data.h"

void SaveData_SetMaskSeed(u32 value)
{
    gSaveData_dwMaskStateHigh = value;
    gSaveData_dwMaskStateLow = value;
}
