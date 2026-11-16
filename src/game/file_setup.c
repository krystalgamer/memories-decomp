/*
 * The file layer's boot setup, one unit covering US [0x800136D4, 0x80013898),
 * merged for #39 from three former units in address order: the empty
 * func_800136D4, which Main_RunFrameServices calls right after
 * File_ServiceTransfers; File_SetPositionTable, which resets the transfer
 * state, initializes the D_800E9DF0 sprite and fills gFile_anLba from the
 * disc paths; and File_WaitForTransfers, the synchronous drain it installs as
 * gFile_BeforePrimaryRequestCallback. The transfer runtime that follows,
 * file_transfer_runtime.c, stays a separate object: its jump table sits at
 * offset 0xC after this unit's printf literal, where one object's .align 3
 * would put it at 0x10.
 */

#include "../types.h"

#include "../psyq/stdio.h"
#include "color_constants.h"
#include "file_names.h"
#include "main.h"

#include "file_transfer.h"

void func_800136D4(void)
{
    volatile u8 unused[9];
}

/* Zero-initialized storage this unit owns. Each build's generated
 * file_setup_commons and file_setup_bss_commons subsegments place it at the
 * retail addresses. */
u8 D_8009B0E0;
SpritePrim D_800E9DF0;

/* Initialized data this unit owns at 0x80090704. The paths are padded to a word boundary
   the way the compiler pads them, and gFile_apszName is the null-terminated
   table File_SetPositionTable reads. The paths are writable arrays, not
   literals: GCC would put literals in .rodata after the table, while retail
   keeps them in .data ahead of it. */
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
 * (src/game/european/file_setup.c) defines its own. */
#ifndef FILE_POSITION_TABLE_Y
#define FILE_POSITION_TABLE_Y 0xD0
#endif

void File_SetPositionTable(void)
{
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
    state->rgb.word = COLOR_RGB24_NEUTRAL_GREY;
    state->uv.word = 0xA000;
    state->extent.word = 0x180018;
    state->cxcy.word = 0xFC0230;

    for (i = 0; i < FILE_POSITION_TABLE_CAPACITY; i++) {
        current = gFile_apszName[i];
        if (current == (u8 *)0) {
            break;
        }
        File_GetPosition(&gFile_anLba[i], (const char *)current);
        printf("%s --> %x\n", current, gFile_anLba[i]);
    }
}

void File_WaitForTransfers(void) {
    for (;;) {
        if (((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             gFile_dwCancelRequestState) == 0) {
            break;
        }
        if ((gFile_dwTransferState & FILE_TRANSFER_STATE_SECONDARY_PENDING) == 0) {
            File_StopActiveRangeTransfer();
        }
        Main_AdvanceFrame();
    }

    while (gFile_dwCancelRequestState != 0) {
        Main_AdvanceFrame();
    }
}
