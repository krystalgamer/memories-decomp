#define DUEL_PACKAGE_STAGE_RAW_ARENAS
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "duel_load_package_stage.h"
#include "password_load_package_stage.h"
#include "file_constants.h"
#include "../unmatched.h"
#include "file_transfer_buffer.h"

/* The package's first sector in WA.MRG. A regional build defines its own. */
#ifndef PASSWORD_PACKAGE_START_SECTOR
#define PASSWORD_PACKAGE_START_SECTOR FILE_WA_PASSWORD_START_SECTOR
#endif

#ifndef PASSWORD_PACKAGE_IMAGE_X
#define PASSWORD_PACKAGE_IMAGE_X 0x100
#endif

#ifndef PASSWORD_PACKAGE_IMAGE_Y
#define PASSWORD_PACKAGE_IMAGE_Y 0xF0
#endif

/* The Password package load. File_RequestPasswordPackage queues the package
   transfer with Password_LoadPackageStage as its stage callback. */

void Password_LoadPackageStage(FileTransferDescriptor *object, s32 mode) {
    switch (mode) {
    case 0:
        object->field_30.h.counter = 0x300;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        gFile_dwTransferState &= 0xFFDDFFFF;
        gFile_dwTransferState |= 0x10000;
        object->done = 2;
        object->phase_size = 64 * FILE_SECTOR_SIZE;
        object->value_08 = gFile_TransferBufferBase;
        object->value_0C = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 1:
        object->phase_size = 4 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->value_0C = gFile_TransferBufferBase;
        object->value_08 = gFile_TransferBufferBase;
        object->done = 1;
        break;

    case 2:
        object->x = PASSWORD_PACKAGE_IMAGE_X;
        object->y = PASSWORD_PACKAGE_IMAGE_Y;
        object->w = 0x100;
        object->h = 0x10;
        LoadImage2((RECT *)object, (u32 *)gFile_TransferBufferBase);
        object->value_0C = (s32)D_801A8000;
        object->value_08 = (s32)D_801A8000;
        object->phase_size = 3 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->done = 1;
        break;

    case 3:
        object->phase_size = 15 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->value_0C = (u32)gFile_pScreenBankBase;
        object->value_08 = (u32)gFile_pScreenBankBase;
        object->done = 1;
        break;
    }
}

void File_RequestPasswordPackage(void){File_RequestAsyncTransfer(0,0,PASSWORD_PACKAGE_START_SECTOR,FILE_WA_PASSWORD_SECTOR_COUNT,Password_LoadPackageStage,0,0);File_WaitForTransfers();}
