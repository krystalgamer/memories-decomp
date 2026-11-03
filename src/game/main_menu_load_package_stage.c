#define D_8009B118_IN_DATA
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "file_package_staging.h"
#include "file_transfer_state.h"
#include "file_transfer_steps.h"
#include "../unmatched.h"
#include "display_resource_banks.h"
#include "file_transfer_buffer.h"

#include "high_memory_addresses.h"

/* Regional value: the main-menu package's first sector in SU.MRG. The US
 * package starts at 0; the European wrapper computes it from gText_bLanguageIndex. */
#ifndef MAIN_MENU_PACKAGE_FIRST_SECTOR
#define MAIN_MENU_PACKAGE_FIRST_SECTOR 0
#endif

/* The Main Menu package load. File_RequestMainMenuPackage queues the
   package from gFile_szSuMrgPath with MainMenu_LoadPackageStage as its stage
   callback. */

void MainMenu_LoadPackageStage(FileTransferDescriptor *object, s32 stage) {
    RECT rect;

    switch (stage) {
    case 0:
        object->field_30.h.counter = 0x200;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        gFile_dwTransferState_abs &= 0xFFDDFFFF;
        gFile_dwTransferState_abs |= 0x10000;
        object->transfer_mode = 2;
        object->phase_size = 64 * FILE_SECTOR_SIZE;
        object->sector_destination = gFile_TransferBufferBase;
        object->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 1:
        object->field_30.h.counter = 0x380;
        object->field_30.h.field_32 = 0;
        object->w = 0x40;
        object->h = 0x10;
        gFile_dwTransferState_abs &= 0xFFDDFFFF;
        gFile_dwTransferState_abs |= 0x10000;
        object->transfer_mode = 2;
        object->phase_size = 32 * FILE_SECTOR_SIZE;
        object->sector_destination = gFile_TransferBufferBase;
        object->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 2:
        object->alternate_sector_destination = (s32)gFile_abPackageStagingBuffer;
        object->sector_destination = (s32)gFile_abPackageStagingBuffer;
        object->phase_size = 2 * FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        object->transfer_mode = 1;
        break;

    case 3:
        rect = *(RECT *)gMainMenu_PackageStageRect;
        LoadImage2(&rect, (u32 *)gFile_abPackageStagingBuffer);
        object->phase_size = 16 * FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        object->alternate_sector_destination = gMainMenu_dwSuModuleBase;
        object->sector_destination = gMainMenu_dwSuModuleBase;
        object->transfer_mode = 1;
        break;

    case 4:
        object->alternate_sector_destination = (s32)gDisplay_abResourceBank1;
        object->sector_destination = (s32)gDisplay_abResourceBank1;
        object->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        object->transfer_mode = 1;
        break;
    }
}

/* The eight bytes at 0x8009B058, the first of the three owners in the
 * 40-byte range the notes call the scattered tail. The movie pipeline's
 * block was carved in #3795 and the model transfer block in #3829, so this
 * completes the range.
 *
 * It holds one object, not two words. MainMenu_LoadPackageStage is its only
 * reader and it takes the whole thing at once, as
 *
 *     rect = *(RECT *)gMainMenu_PackageStageRect;
 *     LoadImage2(&rect, (u32 *)gFile_abPackageStagingBuffer);
 *
 * so the eight bytes are a libgpu RECT. Read that way the values are
 * coherent: x 0, y 240, w 256, h 8, a 256-by-8 strip of VRAM sitting just
 * below a 240-line frame buffer, which is what a staging rectangle for
 * LoadImage2 should look like. Nothing names 0x8009B05C, and no reader
 * touches the second word on its own, so splat's single label for the pair
 * is the object rather than a label that ran long.
 *
 * The definition is still spelled as bytes. The declaration its reader
 * carries is `extern u8 gMainMenu_PackageStageRect[]`, and under -G8 an array is what keeps
 * that access off %gp_rel; a RECT is exactly eight bytes and would sit in
 * small data instead. That is measured rather than assumed: declaring it
 * `extern RECT gMainMenu_PackageStageRect` in MainMenu_LoadPackageStage and reading it
 * without the cast builds an executable of 0x1D07FC bytes against the
 * expected 0x1D0800. So the array spelling there is load bearing, and this
 * definition matches it rather than asserting a type the only consumer does
 * not use. The RECT is recorded here as what the bytes mean, not as how they
 * are declared.
 *
 * No linker alias covers this symbol, so unlike the two blocks after it
 * there is nothing to retire. */

u8 gMainMenu_PackageStageRect[8] = {
    0x00,
    0x00,
    0xF0,
    0x00,
    0x00,
    0x01,
    0x08,
    0x00,
};

void File_RequestMainMenuPackage(void)
{
    File_RequestAsyncTransfer(
        1, gFile_szSuMrgPath, MAIN_MENU_PACKAGE_FIRST_SECTOR, 0x73,
        MainMenu_LoadPackageStage, 0, 0
    );
}
