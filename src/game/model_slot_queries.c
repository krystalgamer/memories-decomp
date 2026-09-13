#include "../types.h"

#include "model.h"
#include "model_slot_queries.h"

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

u16 func_80058EC0(s32 idx)
{
    u8 sub = D_800F2C40[idx].field_BF5;
    return D_800F2C40[idx].field_750[sub].max;
}
