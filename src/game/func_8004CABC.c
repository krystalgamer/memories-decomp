#include "../types.h"

extern u8 *D_8009B458;

s32 func_8004CABC(void)
{
    u8 *object = D_8009B458;
    s32 index;
    u16 count = *(u16 *)(object + 0x7FA);

    for (index = 0; index < count; index++, object += 0x2C) {
        if (object[0x53C] != 1) {
            return 1;
        }
    }

    return 3;
}
