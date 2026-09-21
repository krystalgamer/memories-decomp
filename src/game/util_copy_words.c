#include "../types.h"
#include "util_memory.h"

void Util_CopyWords(u8 *destination, u8 *source, u32 length) {
    s32 index = (length >> 2) - 1;

    for (; index >= 0; index--) {
        *(s32 *)(destination + (u32)&((s32 *)0)[index]) =
            *(s32 *)(source + (u32)&((s32 *)0)[index]);
    }

    switch (length & 3) {
    case 0:
        break;
    case 1:
        *(s32 *)(destination + (u32)&((s32 *)0)[length >> 2]) =
            *(s32 *)(source + (u32)&((s32 *)0)[length >> 2]);
        break;
    case 2:
        *(s32 *)(destination + (u32)&((s32 *)0)[length >> 2]) =
            *(s32 *)(source + (u32)&((s32 *)0)[length >> 2]);
        break;
    case 3:
        *(s32 *)(destination + (u32)&((s32 *)0)[length >> 2]) =
            *(s32 *)(source + (u32)&((s32 *)0)[length >> 2]);
        destination[length - 1] = source[length - 1];
        break;
    }
}
