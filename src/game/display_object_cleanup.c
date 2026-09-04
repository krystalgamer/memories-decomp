#include "../types.h"

extern void func_8004036C(void *);

void func_80039F90(void **objects)
{
    s32 i;

    for (i = 2; i >= 0; i--) {
        func_8004036C(objects[i]);
        objects[i] = 0;
    }
}

void func_80039FD4(u8 *object)
{
    *(s8 *)(object + 0x30) = -1;
    func_80039F90((void **)object);
}
