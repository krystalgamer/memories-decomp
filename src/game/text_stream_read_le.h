#ifndef MEMORIES_DECOMP_TEXT_STREAM_READ_LE_H
#define MEMORIES_DECOMP_TEXT_STREAM_READ_LE_H

#include "../types.h"
#include "../ygo_types.h"
#include "duel_effect.h"

/* Reads the next little-endian halfword from one of the byte streams held at
 * the front of an object and advances that stream past it.
 *
 * The stream is chosen by the signed byte at 0x58, which indexes the array of
 * pointers the object begins with, so a caller switches streams by writing
 * that byte rather than by passing a different object.
 *
 * The parameter is the object that owns the streams, not a stream itself. */
int TextStream_ReadU16LE(DuelEffectChannel *object);

/* Reads one little-endian 32-bit word out of a script stream and advances it.
 *
 * The TextStreamOwner carries 22 stream pointers followed by the signed
 * selector at +0x58. The selected pointer is stepped four bytes on, and the
 * word is assembled from those bytes low-first, so the stream is
 * little-endian here, unlike the sequence readers in the sound path.
 *
 * DuelEffectChannel has the same stream-pointer prefix and calls its selector
 * stream_index; the sole caller casts that wider record to this narrow view.
 *
 * Text_PushFormattedDecimal is the only consumer. */
u32 TextStream_ReadU32LE(TextStreamOwner *object);

#endif
