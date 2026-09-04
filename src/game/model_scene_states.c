#include "../types.h"

extern s8 D_8009AF94;
extern s8 D_8009AF9A;
extern void func_800530C4(void);
extern void func_800533D8(void);
extern void func_80059590(int, int, int, int, int);
extern void func_8005F3B8(int, int, int, int, int);

void func_80059C24(void)
{
    func_800533D8();
    func_800530C4();
    D_8009AF94 = 0x13;
    func_8005F3B8(0, 10000, 0xE00, 0, 0);
    func_80059590(0, 5, 0, 0, 0);
    D_8009AF9A = -1;
}

int func_80059C88(void)
{
    return D_8009AF9A == -2;
}

void func_80059C9C(void)
{
    func_800533D8();
    func_800530C4();
    D_8009AF94 = 20;
    D_8009AF9A = -1;
}

int func_80059CD0(void)
{
    return D_8009AF9A == -2;
}
