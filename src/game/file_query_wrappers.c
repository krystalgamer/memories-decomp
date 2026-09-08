#include "../types.h"

extern int PCopen(int, int, int);
extern int PClseek(int, int, int);
extern void PCclose(int);
extern void func_80059908(int, int, int, int);

int func_8005988C(int value)
{
    int handle = PCopen(value, 0, 0);

    if (handle < 0)
        return -1;
    value = PClseek(handle, 0, 2);
    PCclose(handle);
    return value;
}

void func_800598E4(int first, int second)
{
    func_80059908(first, second, 0, 0);
}
