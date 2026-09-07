#include "../types.h"

extern u32 D_8009AF64;
extern u32 D_8009AF68;

void SaveData_SetMaskSeed(u32 value)
{
    D_8009AF68 = value;
    D_8009AF64 = value;
}
