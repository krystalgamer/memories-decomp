#include "../types.h"

extern u32 gSaveData_dwMaskStateLow;
extern u32 gSaveData_dwMaskStateHigh;

void SaveData_SetMaskSeed(u32 value)
{
    gSaveData_dwMaskStateHigh = value;
    gSaveData_dwMaskStateLow = value;
}
