#include "../types.h"
#include "text_encode_decimal_digits.h"
#include "text_constants.h"
#include "duel_transition_step_table.h"
#include "text_encode_decimal_no_padding.h"

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

void Text_EncodeDecimalNoPadding(s32 value, s32 count_arg, u8 *data_arg) {
    s32 count;
    u8 *data;
    u8 *position;
    u8 current;

    count = count_arg;
    data = data_arg;
    Text_EncodeDecimalDigits(value, count, data);
decrement:
    count = count - 1;
scan:
    if (count <= 0) {
        return;
    }
    position = data + count;
    current = *position;
    count--;
    if (current < TEXT_DECIMAL_RADIX) {
        goto scan;
    }
    count++;
    *position = 0;
    goto decrement;
}
