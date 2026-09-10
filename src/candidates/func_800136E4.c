/* Reclassified from matching_c (#3859). This was
 * src/game/file_set_position_table.c, byte-exact only under
 * gcc_2_8_1_cc_g8_as_g2_split, whose compiler and assembler disagree about
 * small data (GCC -G8, MASPSX -G2). Under gcc_2_8_1_g0, a single threshold,
 * it is 65 instructions against the target's 64, opcode distance 1. The
 * source below is the match, unchanged apart from its include paths. */
#include "../types.h"
#include "../game/file_names.h"
#include "../unmatched.h"
#include "../game/file_transfer.h"

extern u8 D_800E9DF0[];
extern u8 D_8009B0E0;
extern u8 D_80010038[4];

void File_SetPositionTable(void)
{
    s32 *position;
    u8 **name;
    u8 *current;
    s32 i;
    u8 *state;

    File_InitTransferState((s32)gLibrary_aCardArtRecord);

    D_8009B10C = File_WaitForTransfers;
    state = D_800E9DF0;
    *(s16 *)(state + 4) = 0x120;
    *(s16 *)(state + 6) = 0xD0;
    *(s16 *)(state + 0xC) = 0xB;
    *(s32 *)D_800E9DF0 = 0x8000000;
    D_8009B0E0 = 0;
    *(s32 *)(state + 0x14) = 0x808080;
    *(u16 *)(state + 0xE) = 0xA000;
    *(s32 *)(state + 8) = 0x180018;
    *(s32 *)(state + 0x10) = 0xFC0230;

    for (i = 0, position = gFile_anLba, name = gFile_apszName;;) {
        current = *name;
        if (current == (u8 *)0) {
            break;
        }
        File_GetPosition(position, (const char *)current);
        printf(D_80010038, current, *position);
        position++;
        name++;
        i++;
        if (i >= FILE_POSITION_TABLE_CAPACITY) {
            break;
        }
    }
}
