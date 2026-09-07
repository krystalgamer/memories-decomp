#include "../types.h"
#include "text_constants.h"

extern s32 D_80090E0C[];

void Text_EncodeDecimalDigits(s32 arg0, s32 arg1, u8 *arg2) {
    s32 d = D_80090E0C[arg1];
    s32 i;

    i = arg1 - 1;
    do {
        arg2[i] = arg0 / d;
        arg0 -= arg2[i] * d;
        d /= TEXT_DECIMAL_RADIX;
        i--;
    } while (i >= 0);
    i = arg1 - 1;
    while (i > 0) {
        if (arg2[i] != 0) {
            break;
        }
        arg2[i] = TEXT_DECIMAL_BLANK_DIGIT;
        i--;
    }
}
