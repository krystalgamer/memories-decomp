#ifndef MEMORIES_DECOMP_SOUND_SECONDARY_VOICE_H
#define MEMORIES_DECOMP_SOUND_SECONDARY_VOICE_H

#include "../types.h"
#include "sound.h"

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
 * semitone and the fine step, then shifts by the octave. The table is the
 * start of this unit's .rodata. */
extern const u16 gSD_awNotePitchTable[12][128];

/* Computes one secondary sound object's pan position and stereo level pair.
 *
 * `object` is an SDSecondaryObject out of SDSecondaryState::objects; `channel`
 * is the SDSecondaryRecord out of SDSecondaryState::channels that the object's
 * channel_index selects. The two resident callers and the retained note-start
 * candidate select entries in these distinct arrays from the same state base.
 *
 * The pan sum combines the channel's pan (controller 0x0A) with the object's
 * own two pan bytes; the level folds in the channel's volume (controller 7)
 * and expression (controller 0x0B). Results land in the object's `pan`,
 * `level_left` and `level_right`.
 *
 * The function returns nothing; the two declarations that gave it an int
 * return were guesses that never had a value to use. */
void SD_SpatializeSecondaryObject(SDSecondaryObject *object,
                                 SDSecondaryRecord *channel);

void SD_UpdateSecondaryObjectVolumes(void);
s32 SD_CalcPitchBend(SDSecondaryObject *entry, s32 value);

/* A one-call wrapper around SD_ResetSecondaryPlayback. Its three callers are
   the secondary-driver lifecycle entry points: SD_InitSecondaryWorkArea after
   SpuInit, SD_ResetSecondaryRuntime, and SD_Term before SpuQuit. Each one
   calls SD_ResetSecondaryState afterwards. */
void SD_ResetSecondaryPlaybackThunk(void);

void SD_KeyOffSecondaryObject(s32 index);

/* Returns the index of the last eligible secondary object on channel `value`
 * (compared as its low byte) whose 16-bit priority at +0x1E is the smallest
 * seen, or -1 when none is eligible. The note starter SD_KeyOnSecondaryNote calls
 * it with the channel masked to its low byte. */
s32 SD_FindLowestPrioritySecondaryObject(s32 value);

/* Updates the selected object's current channel bookkeeping before reuse. */
s32 SD_PrepareSecondaryObjectReuse(s32 index, s32 value);
/* Selects a reusable secondary object for one channel/variant request. */
s32 SD_SelectSecondaryObject(s32 value, s32 variant);

#endif
