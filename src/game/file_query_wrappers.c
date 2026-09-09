#include "../types.h"
#include "../psyq/libsn.h"
#include "func_80059908.h"

int func_8005988C(int value)
{
    int handle = PCopen((char *)value, 0, 0);

    if (handle < 0)
        return -1;
    value = PClseek(handle, 0, 2);
    PCclose(handle);
    return value;
}

s32 func_800598E4(int first, int second)
{
    return func_80059908(first, second, 0, 0);
}
