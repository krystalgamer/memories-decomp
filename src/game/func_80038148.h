#ifndef MEMORIES_DECOMP_FUNC_80038148_H
#define MEMORIES_DECOMP_FUNC_80038148_H

#include "../types.h"

/* D_80090EAC entry: formats a decimal number into the object's text. The value
 * comes through func_80036D70 as a pointer, the width from the low nibble of the
 * next stream byte, and Text_EncodeDecimalDigits does the conversion into a
 * local buffer before the glyphs are appended. */
void func_80038148(u8 *object);

#endif
