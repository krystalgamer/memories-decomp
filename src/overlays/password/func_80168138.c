#include "../../types.h"

extern void func_8003B6AC(int, int);
extern u8 *func_80035BE4(int, void *, int, int, int, int);
extern void func_80039A14(void *);

void func_80168138(u8 *a)
{
    u8 *object;

    func_8003B6AC(1, 1);
    object = func_80035BE4(1, a + 0xF0, 0x16, 0x18, 0x140, 0xF0);
    object[0x5A] = 0x14;
    object[0x5B] = 0x12;
    func_80039A14(object);
}
