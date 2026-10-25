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

void File_RequestMainMenuPackage(void)
{
    File_RequestAsyncTransfer(
        1, gFile_szSuMrgPath, MAIN_MENU_PACKAGE_FIRST_SECTOR, 0x73,
        MainMenu_LoadPackageStage, 0, 0
    );
}
