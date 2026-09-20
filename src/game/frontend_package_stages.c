#include "../types.h"
#include "display_asset_banks.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "../unmatched.h"

/* The Options package load. File_RequestOptionsPackage queues the package
   transfer with Options_LoadPackageStage as its stage callback, then loads
   a second 0x10-sector block. */

void Options_LoadPackageStage(FileTransferDescriptor *object, s32 mode)
{
    switch (mode) {
    case 0:
        object->field_30.h.field_32 = 0x100;
        object->field_30.h.counter = 0;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4 &= 0xFFDDFFFF;
        object->phase_size = 32 * FILE_SECTOR_SIZE;
        D_8009B0F4 |= 0x10000;
        object->done = 2;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + FILE_SECTOR_SIZE;
        break;

    case 1:
        object->phase_size = FILE_SECTOR_SIZE;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->value_0C = D_8009B118;
        object->value_08 = D_8009B118;
        object->done = 1;
        break;

    case 2:
        object->x = 0x100;
        object->y = 0xF0;
        object->w = 0x100;
        object->h = 4;
        LoadImage2((RECT *)object, (u32 *)D_8009B118);
        object->value_0C = (s32)D_801AF000;
        object->value_08 = (s32)D_801AF000;
        object->phase_size = FILE_SECTOR_SIZE;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->done = 1;
        break;

    case 3:
        object->value_0C = 0x80140000;
        object->value_08 = 0x80140000;
        object->phase_size = 16 * FILE_SECTOR_SIZE;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->done = 1;
        break;
    }
}

void File_RequestOptionsPackage(void){File_RequestAsyncTransfer(0,0,0x2115,0x32,Options_LoadPackageStage,0,0);File_WaitForTransfers();File_RequestAsyncTransfer(0,0,0x2147,0x10,0,0,(int)0x80140000);File_WaitForTransfers();}

void GameOver_LoadPackageStage(FileTransferDescriptor *object, s32 mode)
{
    switch (mode) {
    case 0:
        object->field_30.h.field_32 = 0x100;
        object->field_30.h.counter = 0;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4 &= 0xFFDDFFFF;
        object->phase_size = 48 * FILE_SECTOR_SIZE;
        D_8009B0F4 |= 0x10000;
        object->done = 2;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + FILE_SECTOR_SIZE;
        break;

    case 1:
        object->phase_size = FILE_SECTOR_SIZE;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->value_0C = D_8009B118;
        object->value_08 = D_8009B118;
        object->done = 1;
        break;

    case 2:
        object->x = 0;
        object->y = 0xF0;
        object->w = 0x100;
        object->h = 4;
        LoadImage2((RECT *)object, (u32 *)D_8009B118);
        object->value_0C = (s32)D_801AF000;
        object->value_08 = (s32)D_801AF000;
        object->phase_size = FILE_SECTOR_SIZE;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->done = 1;
        break;
    }
}
