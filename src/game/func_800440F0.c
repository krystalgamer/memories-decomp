#include "../types.h"
#include "../psyq/libapi.h"

extern long gMemCard_aIOEventHandles[];
extern int func_800440B4(int, int);
extern void func_80043D48(long *);

int func_800440F0(int value)
{
    int result;
    if (func_800440B4(value, 1)) {
        func_80043D48(gMemCard_aIOEventHandles);
        _card_info(value);
        result = 1;
    } else {
        result = 0;
    }
    return result;
}
