#ifndef MEMORIES_DECOMP_TEXT_ENCODE_DECIMAL_NO_PADDING_H
#define MEMORIES_DECOMP_TEXT_ENCODE_DECIMAL_NO_PADDING_H

#include "../types.h"

/* Text_EncodeDecimalDigits, then every byte after the first that is not a
 * decimal digit -- the blanks that stand for leading zeros -- is cleared to
 * 0, so the number is written without padding. */
void Text_EncodeDecimalNoPadding(s32 value, s32 count_arg, u8 *data_arg);

#endif
