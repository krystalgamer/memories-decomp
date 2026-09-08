#include "../types.h"
#include "sound_sequence_reader.h"
#include "sound_sequence_values.h"

s32 SD_ReadSequenceU32BE(void *input)
{
    s32 first = SD_ReadSequenceByte(input);
    s32 second = SD_ReadSequenceByte(input);
    s32 third = SD_ReadSequenceByte(input);
    s32 fourth = SD_ReadSequenceByte(input);

    return (fourth & 0xFF) + ((third & 0xFF) << 8) +
           ((second & 0xFF) << 16) + (first << 24);
}

s32 SD_ReadSequenceU16BE(void *input)
{
    s32 high = SD_ReadSequenceByte(input);
    s32 low = SD_ReadSequenceByte(input);

    return (low & 0xFF) | ((high & 0xFF) << 8);
}
