#include "../types.h"
#include "util_memory.h"

void Util_FillMemory(u8 *destination, s32 value, u32 length) {
    s32 index = (length >> 2) - 1;
    s32 fill;

    value &= 0xFF;
    fill = (value << 24) | (value << 16) | (value << 8) | value;

    for (; index >= 0; index--) {
        *(s32 *)(destination + (u32)&((s32 *)0)[index]) = fill;
    }

    switch (length & 3) {
    case 0:
        break;
    case 1:
        *(s32 *)(destination + (u32)&((s32 *)0)[length >> 2]) = fill;
        break;
    case 2:
        *(s32 *)(destination + (u32)&((s32 *)0)[length >> 2]) = fill;
        break;
    case 3:
        *(s32 *)(destination + (u32)&((s32 *)0)[length >> 2]) = fill;
        destination[length - 1] = fill;
        break;
    }
}
