#include "../types.h"
#define D_8009B458_IN_DATA
#include "sound.h"
#include "sound_sequence_reader.h"

#define SD_SEQUENCE_MARKER_SIZE 4

s32 SD_FindMidiTrackChunk(s32 offset)
{
    do {
        if (SD_CompareBytes(
                D_8009AF80,
                gSD_pSecondaryState->field_07DC + offset,
                SD_SEQUENCE_MARKER_SIZE) == 0) {
            return offset + SD_SEQUENCE_MARKER_SIZE;
        }

        offset++;
    } while ((u32)gSD_pSecondaryState->field_07EC >= (u32)offset);

    return -1;
}
