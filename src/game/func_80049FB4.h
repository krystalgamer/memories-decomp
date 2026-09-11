#ifndef MEMORIES_DECOMP_FUNC_80049FB4_H
#define MEMORIES_DECOMP_FUNC_80049FB4_H

#include "../types.h"

/* Computes the SPU pitch word for a note.
 *
 * The first two arguments are one note value already split by its callers as
 * `(s16)v >> 7` and `v & 0x7F`; the last two are the tone's base note and its
 * fine offset. It reduces the difference to an octave and a semitone within
 * it, reads the twelve-entry table at D_80010834 for that semitone, and
 * shifts by the octave.
 *
 * The result goes straight into SpuVoiceAttr's pitch: func_8004A43C
 * assigns it to D_8009B458->voice_attr.pitch and calls SpuSetVoiceAttr. */
s32 func_80049FB4(s32 note_high, s32 note_low, s32 base, s32 offset);

#endif
