#include "../types.h"
#include "func_80036D3C.h"

int func_80036D3C(u8 *object)
{
    u8 **stream = &((u8 **)object)[*(s8 *)(object + 0x58)];
    u8 *current = *stream;
    *stream = current + 2;
    return current[0] | (current[1] << 8);
}
