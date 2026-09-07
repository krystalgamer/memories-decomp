#include "../types.h"
#include "text_constants.h"

extern void Text_EncodeDecimalDigits(s32, s32, u8 *);

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
