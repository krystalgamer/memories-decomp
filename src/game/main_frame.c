#include "../types.h"

extern void func_8001306C(void);
extern void func_80012DB4(void);
extern void func_80012E5C(void);
extern void func_8003CCD8(void);

void func_80012D4C(void)
{
    func_8001306C();
    func_80012DB4();
    func_80012E5C();
    func_8003CCD8();
}

void func_80012D84(int count)
{
    do {
        func_80012D4C();
    } while (--count != 0);
}
