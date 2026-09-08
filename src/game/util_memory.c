#include "../types.h"
#include "util_memory.h"

void Util_CopyWords(u8 *destination, u8 *source, u32 length) {
    s32 index = (length >> 2) - 1;

    for (; index >= 0; index--) {
        *(s32 *)(destination + index * 4) =
            *(s32 *)(source + index * 4);
    }

    switch (length & 3) {
    case 0:
        break;
    case 1:
        *(s32 *)(destination + (length >> 2) * 4) =
            *(s32 *)(source + (length >> 2) * 4);
        break;
    case 2:
        *(s32 *)(destination + (length >> 2) * 4) =
            *(s32 *)(source + (length >> 2) * 4);
        break;
    case 3:
        *(s32 *)(destination + (length >> 2) * 4) =
            *(s32 *)(source + (length >> 2) * 4);
        destination[length - 1] = source[length - 1];
        break;
    }
}

void Util_FillMemory(u8 *destination, s32 value, u32 length) {
    s32 index = (length >> 2) - 1;
    s32 fill;

    value &= 0xFF;
    fill = (value << 24) | (value << 16) | (value << 8) | value;

    for (; index >= 0; index--) {
        *(s32 *)(destination + index * 4) = fill;
    }

    switch (length & 3) {
    case 0:
        break;
    case 1:
        *(s32 *)(destination + (length >> 2) * 4) = fill;
        break;
    case 2:
        *(s32 *)(destination + (length >> 2) * 4) = fill;
        break;
    case 3:
        *(s32 *)(destination + (length >> 2) * 4) = fill;
        destination[length - 1] = fill;
        break;
    }
}
