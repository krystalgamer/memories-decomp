#include "../types.h"
#include "model.h"

int func_8005A878(int arg)
{
    int result = 0;
    if (D_800F2B20.flags == 0) {
        if (arg != 0 || D_800F2B20.mode == 0 || D_800F2B20.field_02 == 0)
            result = 1;
    }
    return result;
}
