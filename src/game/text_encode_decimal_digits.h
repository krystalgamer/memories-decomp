#ifndef MEMORIES_DECOMP_TEXT_ENCODE_DECIMAL_DIGITS_H
#define MEMORIES_DECOMP_TEXT_ENCODE_DECIMAL_DIGITS_H
#include "../types.h"

/* Text_EncodeDecimalDigits, then every byte after the first that is not a
 * decimal digit -- the blanks that stand for leading zeros -- is cleared to
 * 0, so the number is written without padding. */
void Text_EncodeDecimalNoPadding(s32 value, s32 count_arg, u8 *data_arg);

/* Writes value into digits as digit_count decimal digits, least significant last, and
 * replaces leading zeros with TEXT_DECIMAL_BLANK_DIGIT. The buffer must hold
 * digit_count bytes; callers pass a stack array of eight. */
void Text_EncodeDecimalDigits(s32 value, s32 digit_count, u8 *digits);
#endif
