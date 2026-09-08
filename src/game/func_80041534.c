#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

int func_80041534(u8 *object, u8 *data)
{
    int high;
    int low;
    *(u32 *)(object + 4) |= GsROTOFF;
    object[0x22] = data[0];
    *(short *)(object + 0x4A) = (signed char)data[1];
    high = data[3] << 8;
    low = data[2];
    *(u32 *)(object + 0x50) += 4;
    *(short *)(object + 0x48) = high | low;
    return 1;
}
