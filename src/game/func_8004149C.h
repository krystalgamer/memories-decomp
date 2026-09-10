#ifndef MEMORIES_DECOMP_FUNC_8004149C_H
#define MEMORIES_DECOMP_FUNC_8004149C_H

#include "../types.h"

/* Display-object stream opcode 0xFA, entry 5 of D_80090FEC
 * (model_record_tables.c). `t` holds a count byte followed by that many
 * little-endian halfword offsets. One is picked with rand(), and the stream
 * cursor at +0x50 is set to the stream base at +0x54 plus that offset, so the
 * opcode is a random jump. Also zeroes the halfword at +0x58. Returns 1. */
int func_8004149C(u8 *p, u8 *t);

#endif
