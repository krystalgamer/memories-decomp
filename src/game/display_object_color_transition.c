#include "../types.h"

void func_8001D344(u8 *object)
{
    s32 step = *(s16 *)(object + 0x60);
    s32 remaining = 3;
    s32 i = 0;
    u8 *current = object + 0xC;
    u8 *target = object;

    for (; i < 3; i++) {
        s32 value = *current;
        s32 limit = *(s16 *)(target + 0x28);

        if (value < limit) {
            value += step;
            if (value >= limit) {
                value = limit;
                remaining--;
            }
        } else {
            value -= step;
            if (value <= limit) {
                value = limit;
                remaining--;
            }
        }
        *current = value;
        current++;
        target += 2;
    }

    if (remaining == 0) {
        object[0x6C] = 0;
        *(s32 *)(object + 0x24) = 0;
    }
}
