#include "../types.h"

extern s32 D_8009B350;
extern void (*D_8009B340)(void *);

extern s32 func_80037C74(void *);

void func_80038E1C(u8 *object)
{
    object[0x56]++;
    *(u16 *)(object + 0x38) = 0x1000;
    if (func_80037C74(object)) {
        object[0x51] = 4;
    }
    D_8009B350 = 1;
    if (D_8009B340) {
        D_8009B340(object);
    }
}

void func_80038E7C(u8 *object)
{
    u16 flags;

    object[0x58]--;
    if (*(s8 *)(object + 0x58) < 0) {
        flags = *(u16 *)(object + 0x34);
        D_8009B350 = 1;
        flags |= 0x2000;
        *(u16 *)(object + 0x34) = flags;
    }
}
