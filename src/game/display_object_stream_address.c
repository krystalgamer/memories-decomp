#include "../types.h"

typedef struct {
    u8 pad_00[0x54];
    u8 *base;
} DisplayObjectStream;

u32 func_800429A8(const u8 *data)
{
    return (data[1] << 8) | data[0];
}

u8 *func_800429BC(DisplayObjectStream *object, const u8 *data)
{
    return object->base + ((data[1] << 8) | data[0]);
}
