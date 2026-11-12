#ifndef MEMORIES_DECOMP_SOUND_SEQUENCE_READER_H
#define MEMORIES_DECOMP_SOUND_SEQUENCE_READER_H

#include "../types.h"
#include "sound.h"

s32 SD_CompareBytes(u8 *left, u8 *right, s32 count);
s32 SD_FindMidiTrackChunk(s32 offset);
s32 SD_ReadSequenceByte(SDSequenceTrack *reader);
s32 SD_ReadVariableLengthValue(SDSequenceTrack *input);

#endif
