#ifndef MEMORIES_DECOMP_TEXT_ENCODE_DECIMAL_DIGITS_H
#define MEMORIES_DECOMP_TEXT_ENCODE_DECIMAL_DIGITS_H

#include "../types.h"

/* Writes arg0 into arg2 as arg1 decimal digits, least significant last, and
 * replaces leading zeros with TEXT_DECIMAL_BLANK_DIGIT. The buffer must hold
 * arg1 bytes; callers pass a stack array of eight. */
void Text_EncodeDecimalDigits(s32 arg0, s32 arg1, u8 *arg2);

#endif
