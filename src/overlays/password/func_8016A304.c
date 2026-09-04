#include "../../types.h"

extern u8 gPassword_abDigits[];
extern s32 D_801A8008[];

s32 func_8016A304(void)
{
    s32 packed = 0;
    s32 *entry = D_801A8008;
    s32 index;
    s32 i;

    for (i = 0; i < 8; i++) {
        packed <<= 4;
        packed |= gPassword_abDigits[i];
    }

    index = 1;
    for (;;) {
        if (entry[0] == -1) {
            return 0;
        }
        if (packed == entry[1]) {
            return index;
        }
        entry += 2;
        index++;
    }
}
