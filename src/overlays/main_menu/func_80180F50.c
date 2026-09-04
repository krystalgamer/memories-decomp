#include "../../types.h"

extern void func_8004036C(void);

void func_80180F50(u8 *object)
{
    s32 r;
    s32 g;
    s32 b;

    if ((*(s32 *)(object + 0xC) & 0xFFFFFF) != 0) {
        r = object[0xC] - 8;
        if (r < 0) {
            r = 0;
        }
        object[0xC] = r;
        g = object[0xD] - 8;
        if (g < 0) {
            g = 0;
        }
        object[0xD] = g;
        b = object[0xE] - 8;
        if (b < 0) {
            b = 0;
        }
        object[0xE] = b;
    } else {
        func_8004036C();
    }
}
