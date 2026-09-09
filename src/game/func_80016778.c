#include "../types.h"
#include "display_object_config.h"
#include "func_80016778.h"

void func_80016778(void *object, u32 value)
{
    ((DisplayObjectConfig *)object)->field_69 = value >> 31;
}
