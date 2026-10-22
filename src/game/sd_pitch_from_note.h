#ifndef MEMORIES_DECOMP_SD_PITCH_FROM_NOTE_H
#define MEMORIES_DECOMP_SD_PITCH_FROM_NOTE_H

#include "../types.h"

/* Computes the SPU pitch word for a note.
 *
 * The first two arguments are one note value already split by its callers as
 * `(s16)v >> 7` and `v & 0x7F`; the last two are the tone's base note and its
 * fine offset. It reduces the difference to an octave and a semitone within
 * it, reads the twelve-entry table at gSD_awNotePitchTable for that semitone, and
 * shifts by the octave.
 *
 * The result goes straight into SpuVoiceAttr's pitch: SD_UpdateSecondaryObjectPitch
 * assigns it to gSD_pSecondaryState->voice_attr.pitch and calls SpuSetVoiceAttr. */
s32 SD_PitchFromNote(s32 note_high, s32 note_low, s32 base, s32 offset);

/* Twelve semitone rows of 128 fine-tune steps: the SPU pitch word for each
 * semitone of the base octave. SD_PitchFromNote indexes it by the difference's
 * semitone and the fine step, then shifts by the octave. */
extern u16 gSD_awNotePitchTable[12][128];

#endif
