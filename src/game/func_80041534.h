#ifndef MEMORIES_DECOMP_FUNC_80041534_H
#define MEMORIES_DECOMP_FUNC_80041534_H

#include "../types.h"

/* Display-object stream opcode 0xF9, entry 6 of D_80090FEC
 * (model_record_tables.c). It reads four operand bytes: the first into
 * +0x22, the second sign-extended into the halfword at +0x4A, and the third
 * and fourth as a little-endian halfword into +0x48. It also raises GsROTOFF
 * in the attribute word at +4 and advances the stream cursor at +0x50 by 4.
 * Returns 1. */
int func_80041534(u8 *object, u8 *data);

#endif
