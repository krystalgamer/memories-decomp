#include "../types.h"

/* Zero-initialized storage this unit owns at 0x8009B0E0-0x8009B0E8, defined in
 * address order ahead of the headers that declare it (#2602). */
u8 D_8009B0E0;
static u8 D_8009B0E1;
static u16 D_8009B0E2;
static u32 D_8009B0E4;

#include "../psyq/stdio.h"
#include "color_constants.h"
#include "file_names.h"

/* Zero-initialized storage this unit owns at 0x800E9DF0-0x800E9E18, defined in
 * address order ahead of the headers that declare it (#2602). */
SpritePrim D_800E9DF0;

#include "file_transfer.h"

/* Initialized data this unit owns at 0x80090704. The paths are padded to a word boundary
   the way the compiler pads them, and gFile_apszName is the null-terminated
   table File_SetPositionTable reads. */
u8 gFile_szDiscWaMrgPath[20] = "\\DATA\\WA_MRG.MRG;1";
u8 gFile_szDiscSuMrgPath[16] = "\\DATA\\SU.MRG;1";
u8 gFile_szDiscModelMrgPath[20] = "\\DATA\\MODEL.MRG;1";
u8 gFile_szDiscMovieStrPath[20] = "\\DATA\\MOVIE.STR;1";
u8 gFile_szDiscSdSeDatPath[20] = "\\DATA\\SD_SE.DAT;1";
u8 gFile_szDiscSdBgmDatPath[20] = "\\DATA\\SD_BGM.DAT;1";
u8 gFile_szDiscMasterXaPath[20] = "\\DATA\\MASTER.XA;1";

u8 *gFile_apszName[8] = {
    gFile_szDiscWaMrgPath,
    gFile_szDiscSuMrgPath,
    gFile_szDiscModelMrgPath,
    gFile_szDiscMovieStrPath,
    gFile_szDiscSdSeDatPath,
    gFile_szDiscSdBgmDatPath,
    gFile_szDiscMasterXaPath,
    (u8 *)0,
};

/* Regional value: the table's y position (0xD0 -> 0xE0). The European build
 * (src/game/european/) defines its own. */
#ifndef FILE_POSITION_TABLE_Y
#define FILE_POSITION_TABLE_Y 0xD0
#endif

void File_SetPositionTable(void)
{
    s32 *position;
    u8 **name;
    u8 *current;
    s32 i;
    SpritePrim *state;

    File_InitTransferState((s32)gLibrary_aCardArtRecord);

    gFile_BeforePrimaryRequestCallback = File_WaitForTransfers;
    state = &D_800E9DF0;
    state->xy.h.x = 0x120;
    state->xy.h.y = FILE_POSITION_TABLE_Y;
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
        printf((const char *)gFile_szPositionFormat, current, *position);
        position++;
        name++;
        i++;
    }
}
