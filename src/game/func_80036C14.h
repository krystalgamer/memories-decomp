#ifndef MEMORIES_DECOMP_FUNC_80036C14_H
#define MEMORIES_DECOMP_FUNC_80036C14_H

#include "../types.h"

/* Appends one 0x1C-byte packet to the buffer the object holds at 0x20, then
 * advances that pointer past it.
 *
 * The second argument is a tagged value, not a plain number: the flags at
 * 0x34 decide how it is read. With 0x80 it is stored as-is, with 0x100 only
 * bits 20..27 are used, and otherwise it is masked with 0x8000FFFF and
 * written as a word. Two of those three paths return without emitting
 * anything when the value comes out zero, so a caller cannot assume a packet
 * was appended, and nothing is returned to say whether one was. */
void func_80036C14(u8 *p, s32 a);

#endif
