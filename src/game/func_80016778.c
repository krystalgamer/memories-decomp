#include "../types.h"
#include "func_80016778.h"

typedef struct {
    u8 pad_00[0x69];
    u8 field_69;
} Object;

void func_80016778(void *object, u32 value)
{
    ((Object *)object)->field_69 = value >> 31;
}
