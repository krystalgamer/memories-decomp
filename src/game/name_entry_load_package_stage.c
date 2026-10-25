#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "name_entry_load_package_stage.h"
#include "file_constants.h"
#include "../unmatched.h"
#include "file_transfer_buffer.h"

/* The package's first sector in WA.MRG. A regional build defines its own. */
#ifndef NAME_ENTRY_PACKAGE_START_SECTOR
#define NAME_ENTRY_PACKAGE_START_SECTOR FILE_WA_NAME_ENTRY_START_SECTOR
#endif

#ifndef NAME_ENTRY_PACKAGE_IMAGE_X
#define NAME_ENTRY_PACKAGE_IMAGE_X 0x100
#endif

#ifndef NAME_ENTRY_PACKAGE_IMAGE_Y
#define NAME_ENTRY_PACKAGE_IMAGE_Y 0xF0
#endif

/* The Name Entry package load. File_RequestNameEntryPackage queues the
   package transfer with NameEntry_LoadPackageStage as its stage callback. */

void NameEntry_LoadPackageStage(FileTransferDescriptor *object, s32 mode)
{
    s32 one;
    s32 v;
    s32 w;
    s32 d;
    s32 m;
    s32 v2;
    s32 w2;
    s32 d2;
    s32 k;
    s32 c;
    s32 hun;
    s32 n;
    s32 x;
    u8 *y;

    /* Direct mode and y members change old-GCC commoning/register allocation. */
    one = 1;

    if (mode == one) {
        goto m1;
    }
    if (mode < 2) {
        if (mode == 0) {
            goto m0;
        }
        return;
    }
    if (mode == 2) {
        goto m2;
    }
    if (mode == 3) {
        goto m3;
    }
    return;

m0:
    m = 0xFFDDFFFF;
    do {
        k = 0x18000;
    } while (0);
    c = 0x100;
    object->field_30.h.counter = c;
    object->field_30.h.field_32 = c;
    v = gFile_dwTransferState;
    object->w = 0x40;
    gFile_dwTransferState = v & m;
    w = gFile_dwTransferState;
    n = 0x10000;
    do {
        *(s32 *)&object->phase_size = k;
        gFile_dwTransferState = w | n;
        object->transfer_mode = 2;
        d = gFile_TransferBufferBase;
        object->h = 0x10;
    } while (0);
    object->sector_destination = d;
    object->alternate_sector_destination = d + FILE_SECTOR_SIZE;
    return;

m1:
    object->field_30.h.counter = 0x1C0;
    object->field_30.h.field_32 = 0x100;
    object->w = 0x40;
    v2 = gFile_dwTransferState;
    object->h = 0x10;
    gFile_dwTransferState = v2 & 0xFFDDFFFF;
    w2 = gFile_dwTransferState;
    gFile_dwTransferState = w2 | 0x10000;
    object->transfer_mode = 2;
    d2 = gFile_TransferBufferBase;
    *(s32 *)&object->phase_size = 16 * FILE_SECTOR_SIZE;
    object->sector_destination = d2;
    object->alternate_sector_destination = d2 + FILE_SECTOR_SIZE;
    return;

m2:
    m = 0xFFDCFFFF;
    *(s32 *)&object->phase_size = FILE_SECTOR_SIZE;
    x = gFile_dwTransferState & m;
    y = (u8 *)gFile_TransferBufferBase;
    goto tail;

m3:
#ifdef VERSION_EUROPE_NAME_ENTRY_PACKAGE_IMAGE
    do {
        hun = NAME_ENTRY_PACKAGE_IMAGE_X;
    } while (0);
    object->x = hun;
    *(u16 *)&object->y = NAME_ENTRY_PACKAGE_IMAGE_Y;
    k = gFile_TransferBufferBase;
    object->w = 0x100;
#else
    do {
        hun = 0x100;
    } while (0);
    *(u16 *)&object->y = 0xF0;
    k = gFile_TransferBufferBase;
    object->x = hun;
    object->w = hun;
#endif
    object->h = 4;
    LoadImage2((RECT *)object, (u32 *)k);
    m = 0xFFDCFFFF;
    *(s32 *)&object->phase_size = 15 * FILE_SECTOR_SIZE;
    x = gFile_dwTransferState & m;
    y = gFile_pScreenBankBase;

tail:
    gFile_dwTransferState = x;
    object->alternate_sector_destination = (s32)y;
    object->sector_destination = (s32)y;
    object->transfer_mode = one;
}

void File_RequestNameEntryPackage(void){File_RequestAsyncTransfer(0,0,NAME_ENTRY_PACKAGE_START_SECTOR,FILE_WA_NAME_ENTRY_SECTOR_COUNT,NameEntry_LoadPackageStage,0,0);File_WaitForTransfers();}
