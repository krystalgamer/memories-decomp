#include "../types.h"
#define D_8009B458_IN_DATA
#include "sound.h"
#include "sound_sequence_reader.h"

#define SD_SEQUENCE_MARKER_SIZE 4

s32 SD_FindMidiTrackChunk(s32 offset)
{
    do {
        if (SD_CompareBytes(
                gSD_szMidiTrackMarker,
                gSD_pSecondaryState->sequence_read_base + offset,
                SD_SEQUENCE_MARKER_SIZE) == 0) {
            return offset + SD_SEQUENCE_MARKER_SIZE;
        }

        offset++;
    } while ((u32)gSD_pSecondaryState->sequence_read_limit >= (u32)offset);

    return -1;
}
