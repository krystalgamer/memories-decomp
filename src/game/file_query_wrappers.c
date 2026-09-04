#include "../types.h"

extern int func_80073704(int, int, int);
extern int func_80073734(int, int, int);
extern void func_80073724(int);
extern void func_80059908(int, int, int, int);

int func_8005988C(int value)
{
    int handle = func_80073704(value, 0, 0);

    if (handle < 0)
        return -1;
    value = func_80073734(handle, 0, 2);
    func_80073724(handle);
    return value;
}

void func_800598E4(int first, int second)
{
    func_80059908(first, second, 0, 0);
}
