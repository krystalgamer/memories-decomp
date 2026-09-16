#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_STREAM_STATE_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_STREAM_STATE_H

#include "../types.h"
#include "../ygo_types.h"

s32 DisplayObjectStream_Stop(
    DisplayObjectStreamState *object,
    const u8 *data
);
s32 DisplayObjectStream_ResetOffset(
    DisplayObjectStreamState *object,
    const u8 *data
);
s32 DisplayObjectStream_Noop(
    DisplayObjectStreamState *object,
    const u8 *data
);
s32 func_8004143C(DisplayObjectStreamState *object, const u8 *data);
s32 func_80041464(DisplayObjectStreamState *object, const u8 *data);

/* Display-object stream opcode 0xFA, entry 5 of D_80090FEC
 * (model_record_tables.c). `data` holds a count byte followed by that many
 * little-endian halfword offsets. One is picked with rand(), and the stream
 * cursor at +0x50 is set to the stream base at +0x54 plus that offset, so the
 * opcode is a random jump. Also zeroes the halfword at +0x58. Returns 1. */
s32 func_8004149C(DisplayObjectStreamState *object, const u8 *data);

/* Display-object stream opcode 0xF9, entry 6 of D_80090FEC
 * (model_record_tables.c). It reads four operand bytes: the first into
 * field_22, the second sign-extended into field_4A, and the third and fourth
 * as a little-endian halfword into field_48. It also raises GsROTOFF in the
 * attribute word `flags` and advances `current` by 4. Returns 1. */
s32 func_80041534(DisplayObjectStreamState *object, const u8 *data);

#endif
