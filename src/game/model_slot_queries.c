#include "../types.h"

#include "model.h"

s32 func_80058E3C(s32 index)
{
    return D_800F2C40[index].field_E0D;
}

s32 func_80058E68(s32 index)
{
    return D_800F2C40[index].field_BF5;
}

s32 func_80058E94(s32 index)
{
    return D_800F2C40[index].field_E06 >> 4;
}
