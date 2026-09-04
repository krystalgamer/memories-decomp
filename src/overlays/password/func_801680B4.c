#include "../../types.h"

extern void *func_8004002C(void);
extern u8 *func_800400AC(void *, int);
extern void func_800404CC(void *, int, int, int, int, int, int, int);

u8 *func_801680B4(int a, int b)
{
    u8 *object;

    object = func_800400AC(func_8004002C(), 2);
    func_800404CC(object, a, b, 0, 0, 0, 0x17, 0x101);
    *(u16 *)(object + 8) = *(u16 *)(object + 8) | 8;
    return object;
}
