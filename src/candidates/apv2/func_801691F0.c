#include "../../types.h"

u32 func_801691F0(u8 *p)
{
    u16 max;
    u16 min;
    s32 i;
    s32 j;
    s32 first;

    max = 0;
    min = 7;
    for (i = 14; i >= 0; i--, p++) {
        first = 1;
        for (j = 7; j >= 0; j--) {
            if ((*p >> j) & 1) {
                if (first) {
                    if (max < j) {
                        max = j;
                    }
                    if (j < min) {
                        min = j;
                    }
                    first = 0;
                } else if (j < min) {
                    min = j;
                }
            }
        }
    }
    return (min << 16) | max;
}
