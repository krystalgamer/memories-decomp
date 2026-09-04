#include "../types.h"

typedef struct {
    u8 pad_00[4];
    u32 flags;
    u8 pad_08[0x48];
    u8 *current;
    u8 *base;
    s16 field_58;
    s16 field_5A;
} DisplayObjectStreamState;

s32 func_8004141C(DisplayObjectStreamState *object)
{
    object->field_5A = 0;
    return -1;
}

s32 func_80041428(DisplayObjectStreamState *object)
{
    object->field_58 = 0;
    return 1;
}

s32 func_80041434(void)
{
    return 1;
}

s32 func_8004143C(DisplayObjectStreamState *object, const u8 *data)
{
    object->field_58 = 0;
    object->current = object->base + ((data[1] << 8) | data[0]);
    return 1;
}

s32 func_80041464(DisplayObjectStreamState *object, const u8 *data)
{
    object->flags ^= 0x800000;
    object->field_58 = 0;
    object->current = object->base + ((data[1] << 8) | data[0]);
    return 1;
}
