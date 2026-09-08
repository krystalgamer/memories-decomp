#ifndef MEMORIES_DECOMP_SOUND_SEQUENCE_READER_H
#define MEMORIES_DECOMP_SOUND_SEQUENCE_READER_H

#include "../types.h"

s32 SD_CompareBytes(u8 *arg0, u8 *arg1, s32 count);
s32 SD_ReadSequenceByte(void *reader);
s32 SD_ReadVariableLengthValue(void *input);

#endif
