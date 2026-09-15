#ifndef MEMORIES_DECOMP_FUNC_80036D70_H
#define MEMORIES_DECOMP_FUNC_80036D70_H

#include "../types.h"
#include "../ygo_types.h"

/* Reads one little-endian 32-bit word out of a script stream and advances it.
 *
 * The TextStreamOwner carries 22 stream pointers followed by the signed
 * selector at +0x58. The selected pointer is stepped four bytes on, and the
 * word is assembled from those bytes low-first, so the stream is
 * little-endian here, unlike the sequence readers in the sound path.
 *
 * DuelEffectChannel has the same stream-pointer prefix and calls its selector
 * stream_58; the sole caller casts that wider record to this narrow view.
 *
 * func_80038148 is the only consumer. */
u32 func_80036D70(TextStreamOwner *object);

#endif
