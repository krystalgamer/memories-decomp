#ifndef MEMORIES_DECOMP_FUNC_80036D3C_H
#define MEMORIES_DECOMP_FUNC_80036D3C_H

#include "../types.h"

/* Reads the next little-endian halfword from one of the byte streams held at
 * the front of an object and advances that stream past it.
 *
 * The stream is chosen by the signed byte at 0x58, which indexes the array of
 * pointers the object begins with, so a caller switches streams by writing
 * that byte rather than by passing a different object.
 *
 * The parameter is the object that owns the streams, not a stream itself. */
int func_80036D3C(u8 *object);

#endif
