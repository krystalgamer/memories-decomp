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
    SpritePrim *state;

    File_InitTransferState((s32)gLibrary_aCardArtRecord);

    D_8009B10C = File_WaitForTransfers;
    state = &D_800E9DF0;
    state->xy.h.x = 0x120;
    state->xy.h.y = 0xD0;
    state->tpage = 0xB;
    D_800E9DF0.attribute = 0x8000000;
    D_8009B0E0 = 0;
    state->rgb = COLOR_RGB24_NEUTRAL_GREY;
    state->uv.word = 0xA000;
    state->extent.word = 0x180018;
    state->cxcy.word = 0xFC0230;

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
