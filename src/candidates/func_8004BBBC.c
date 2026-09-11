/* Reclassified from matching_c (#3859). This was
 * src/game/sound_sequence_marker_scan.c, byte-exact only under
 * gcc_2_8_1_cc_g8_as_g0_split, whose compiler and assembler disagree about
 * small data (GCC -G8, MASPSX -G0). Under gcc_2_8_1_g0_split, a single
 * threshold, it is 29 instructions against the target's 28, opcode distance
 * 3. The source below is the match, unchanged apart from its include paths. */
#include "../types.h"
#include "../game/sound.h"
#include "../unmatched.h"
#include "../game/sound_sequence_reader.h"

#define SD_SEQUENCE_MARKER_SIZE 4

extern u8 D_8009AF80[];

s32 SD_FindMidiTrackChunk(s32 offset)
{
    do {
        if (SD_CompareBytes(
                D_8009AF80,
                D_8009B458->field_07DC + offset,
                SD_SEQUENCE_MARKER_SIZE) == 0) {
            return offset + SD_SEQUENCE_MARKER_SIZE;
        }

        offset++;
    } while ((u32)D_8009B458->field_07EC >= (u32)offset);

    return -1;
}
