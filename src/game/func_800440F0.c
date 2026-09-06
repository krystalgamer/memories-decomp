#include "../types.h"

extern void *gMemCard_aIOEventHandles[];
extern int func_800440B4(int, int);
extern void func_80043D48(void **);
extern void func_8008B330(int);

int func_800440F0(int value)
{
    int result;
    if (func_800440B4(value, 1)) {
        func_80043D48(gMemCard_aIOEventHandles);
        func_8008B330(value);
        result = 1;
    } else {
        result = 0;
    }
    return result;
}
