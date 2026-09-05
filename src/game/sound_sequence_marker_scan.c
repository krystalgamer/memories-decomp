#include "../types.h"
#include "sound.h"

#define SD_SEQUENCE_MARKER_SIZE 4

extern u8 D_8009AF80[];
extern s32 func_8004BAA0(u8 *arg0, u8 *arg1, s32 count);

s32 SD_FindMidiTrackChunk(s32 offset)
{
    do {
        if (func_8004BAA0(
                D_8009AF80,
                D_8009B458->field_07DC + offset,
                SD_SEQUENCE_MARKER_SIZE) == 0) {
            return offset + SD_SEQUENCE_MARKER_SIZE;
        }

        offset++;
    } while ((u32)D_8009B458->field_07EC >= (u32)offset);

    return -1;
}
