#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_STREAM_STATE_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_STREAM_STATE_H

#include "../types.h"
#include "../ygo_types.h"

s32 func_8004141C(DisplayObjectStreamState *object);
s32 func_80041428(DisplayObjectStreamState *object);
s32 func_80041434(void);
s32 func_8004143C(DisplayObjectStreamState *object, const u8 *data);
s32 func_80041464(DisplayObjectStreamState *object, const u8 *data);

/* Display-object stream opcode 0xFA, entry 5 of D_80090FEC
 * (model_record_tables.c). `t` holds a count byte followed by that many
 * little-endian halfword offsets. One is picked with rand(), and the stream
 * cursor at +0x50 is set to the stream base at +0x54 plus that offset, so the
 * opcode is a random jump. Also zeroes the halfword at +0x58. Returns 1. */
int func_8004149C(u8 *p, u8 *t);

/* Display-object stream opcode 0xF9, entry 6 of D_80090FEC
 * (model_record_tables.c). It reads four operand bytes: the first into
 * +0x22, the second sign-extended into the halfword at +0x4A, and the third
 * and fourth as a little-endian halfword into +0x48. It also raises GsROTOFF
 * in the attribute word at +4 and advances the stream cursor at +0x50 by 4.
 * Returns 1. */
int func_80041534(u8 *object, u8 *data);

#endif
