#ifndef MEMORIES_DECOMP_TEXT_STREAM_COMMANDS_H
#define MEMORIES_DECOMP_TEXT_STREAM_COMMANDS_H

#include "../types.h"

/* Three text stream command handlers, all taking the object that owns the
 * streams.
 *
 * func_80037CE0 takes a volatile pointer. That is not decoration: the body
 * reads the halfword at 0x34 through a volatile access and pins two locals to
 * $2 and $3, so the qualifier is part of how the function is written rather
 * than a claim about the caller's storage.
 *
 * func_80037D2C and func_80037D6C both advance one of the byte streams held
 * at the front of the object, chosen by the signed byte at 0x58. */
void func_80037CE0(volatile u8 *object);
void func_80037D2C(u8 *object);
void func_80037D6C(u8 *object);

#endif
