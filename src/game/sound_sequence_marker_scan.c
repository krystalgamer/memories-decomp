#include "../types.h"

extern u8 D_8009AF80[];
extern u8 *D_8009B458;
extern s32 func_8004BAA0(u8 *arg0, s32 arg1, s32 count);

s32 func_8004BBBC(s32 offset)
{
    do {
        if (func_8004BAA0(
                D_8009AF80,
                *(s32 *)(D_8009B458 + 0x7DC) + offset,
                4) == 0) {
            return offset + 4;
        }

        offset++;
    } while (*(u32 *)(D_8009B458 + 0x7EC) >= (u32)offset);

    return -1;
}
