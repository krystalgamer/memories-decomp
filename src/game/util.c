#include "../types.h"
#include "../psyq/rand.h"
#include "util.h"
#include "text_constants.h"
#include "duel_transition_step_table.h"

void Util_CopyWords(u8 *destination, u8 *source, u32 length) {
    s32 index = (length >> 2) - 1;

    for (; index >= 0; index--) {
        *(s32 *)(destination + (u32)&((s32 *)0)[index]) =
            *(s32 *)(source + (u32)&((s32 *)0)[index]);
    }

    switch (length & 3) {
    case 0:
        break;
    case 1:
        *(s32 *)(destination + (u32)&((s32 *)0)[length >> 2]) =
            *(s32 *)(source + (u32)&((s32 *)0)[length >> 2]);
        break;
    case 2:
        *(s32 *)(destination + (u32)&((s32 *)0)[length >> 2]) =
            *(s32 *)(source + (u32)&((s32 *)0)[length >> 2]);
        break;
    case 3:
        *(s32 *)(destination + (u32)&((s32 *)0)[length >> 2]) =
            *(s32 *)(source + (u32)&((s32 *)0)[length >> 2]);
        destination[length - 1] = source[length - 1];
        break;
    }
}

void Util_FillMemory(u8 *destination, s32 value, u32 length) {
    s32 index = (length >> 2) - 1;
    s32 fill;

    value &= 0xFF;
    fill = (value << 24) | (value << 16) | (value << 8) | value;

    for (; index >= 0; index--) {
        *(s32 *)(destination + (u32)&((s32 *)0)[index]) = fill;
    }

    switch (length & 3) {
    case 0:
        break;
    case 1:
        *(s32 *)(destination + (u32)&((s32 *)0)[length >> 2]) = fill;
        break;
    case 2:
        *(s32 *)(destination + (u32)&((s32 *)0)[length >> 2]) = fill;
        break;
    case 3:
        *(s32 *)(destination + (u32)&((s32 *)0)[length >> 2]) = fill;
        destination[length - 1] = fill;
        break;
    }
}

void Text_EncodeDecimalDigits(s32 value, s32 digit_count, u8 *digits) {
    s32 d = gText_DecimalDivisorView[digit_count];
    s32 i;

    i = digit_count - 1;
    do {
        digits[i] = value / d;
        value -= digits[i] * d;
        d /= TEXT_DECIMAL_RADIX;
        i--;
    } while (i >= 0);
    i = digit_count - 1;
    while (i > 0) {
        if (digits[i] != 0) {
            break;
        }
        digits[i] = TEXT_DECIMAL_BLANK_DIGIT;
        i--;
    }
}

void Text_EncodeDecimalNoPadding(s32 value, s32 count, u8 *data) {
    Text_EncodeDecimalDigits(value, count, data);
    for (;;) {
        count--;
        if (count <= 0) {
            return;
        }
        if (data[count] >= TEXT_DECIMAL_RADIX) {
            data[count] = 0;
        }
    }
}

s32 Rand_GetInterval(s32 divisor) { return rand() % divisor; }
