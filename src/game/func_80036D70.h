#ifndef MEMORIES_DECOMP_FUNC_80036D70_H
#define MEMORIES_DECOMP_FUNC_80036D70_H

#include "../types.h"

/* Reads one little-endian 32-bit word out of a script stream and advances it.
 *
 * The object carries several stream pointers; the signed byte at +0x58
 * selects which one, and that pointer is stepped four bytes on. The word is
 * assembled from the four bytes low-first, so the stream is little-endian
 * here, unlike the sequence readers in the sound path.
 *
 * The +0x58 selector is the same field the text command family calls
 * stream_58 on DuelEffectChannel, reached there as *(s8 *)(p + 0x58) and as
 * o->stream_58. This unit spells its parameter as a byte pointer and does not
 * claim the channel type.
 *
 * func_80038148.c is the only consumer, and its local extern already agreed
 * with this in every part but the spelling: u32 against unsigned int and
 * u8 * against unsigned char *, which are the same types. */
unsigned int func_80036D70(unsigned char *object);

#endif
