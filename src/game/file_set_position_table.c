#include "../types.h"
#include "../psyq/stdio.h"
#include "color_constants.h"
#include "file_names.h"
#define D_8009B10C_IN_DATA
#include "file_transfer.h"

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
    *(s32 *)(state + 0x14) = COLOR_RGB24_NEUTRAL_GREY;
    *(u16 *)(state + 0xE) = 0xA000;
    *(s32 *)(state + 8) = 0x180018;
    *(s32 *)(state + 0x10) = 0xFC0230;

    i = 0;
    position = gFile_anLba;
    name = gFile_apszName;
    while (i < FILE_POSITION_TABLE_CAPACITY) {
        current = *name;
        if (current == (u8 *)0) {
            break;
        }
        File_GetPosition(position, (const char *)current);
        printf((const char *)D_80010038, current, *position);
        position++;
        name++;
        i++;
    }
}
