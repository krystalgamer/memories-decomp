#include "../types.h"
#include "func_80058938.h"
#include "model.h"

void func_80059AF8(
    s32 arg0, s32 arg1, ModelTintColor arg2, ModelTintColor arg3, s32 arg4)
{
    arg2.b3 = arg1 & 0x7F;
    func_80058938(arg0, 0, arg2, arg3, arg4, 0);
}
