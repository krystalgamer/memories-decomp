#include "../types.h"

void func_8005B5FC(s32 *destination, s32 value, u32 count)
{
    while (count-- != 0) {
        *destination++ = value;
    }
}

void func_8005B620(s32 *destination, const s32 *source, u32 count)
{
    while (count-- != 0) {
        *destination++ = *source++;
    }
}
