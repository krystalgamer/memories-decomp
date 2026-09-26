#include "../types.h"
#include "display_object_helpers.h"

u32 DisplayObjectStream_ReadU16LE(const u8 *data)
{
    return (data[1] << 8) | data[0];
}

u8 *DisplayObjectStream_ResolveOffset(
    DisplayObjectStream *object,
    const u8 *data
)
{
    return object->base + ((data[1] << 8) | data[0]);
}
