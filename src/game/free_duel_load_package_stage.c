#define D_8009B118_IN_DATA
#include "../types.h"
#include "display_asset_banks.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#define D_800101D8_IN_DATA
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "free_duel_load_package_stage.h"
#include "../unmatched.h"
#include "display_resource_banks.h"
#include "file_transfer_buffer.h"

#define HIGH_MEMORY_ADDRESSES_BASE_IN_DATA
#include "high_memory_addresses.h"
#include "shared_work_base.h"

/* Regional values, which the European wrapper defines for itself: the
   sectors mode 0 transfers (32 in the US build, 48 in the European one)
   and where mode 2 uploads its image row, (0, 0xF0) in the US build and
   (0x280, 0xD0) in the European one. */
#ifndef FREE_DUEL_STAGE0_SECTORS
#define FREE_DUEL_STAGE0_SECTORS 32
#endif
#ifndef FREE_DUEL_IMAGE_X
#define FREE_DUEL_IMAGE_X 0
#endif
#ifndef FREE_DUEL_IMAGE_Y
#define FREE_DUEL_IMAGE_Y 0xF0
#endif

void FreeDuel_LoadPackageStage(FileTransferDescriptor *object, s32 mode) {
    switch (mode) {
    case 0:
        object->field_30.h.counter = 0;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        gFile_dwTransferState_abs &= 0xFFDDFFFF;
        gFile_dwTransferState_abs |= 0x10000;
        object->transfer_mode = 2;
        object->phase_size = FREE_DUEL_STAGE0_SECTORS * FILE_SECTOR_SIZE;
        object->sector_destination = gFile_TransferBufferBase;
        object->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 1:
        object->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        object->alternate_sector_destination = gFile_TransferBufferBase;
        object->sector_destination = gFile_TransferBufferBase;
        object->transfer_mode = 1;
        break;

    case 2:
        object->x = FREE_DUEL_IMAGE_X;
        object->y = FREE_DUEL_IMAGE_Y;
        object->w = 0x100;
        object->h = 4;
        LoadImage2((RECT *)object, (u32 *)gFile_TransferBufferBase);
        object->alternate_sector_destination = (s32)gDisplay_abResourceBank0;
        object->sector_destination = (s32)gDisplay_abResourceBank0;
        object->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        object->transfer_mode = 1;
        break;

    case 3:
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        object->phase_size = 48 * FILE_SECTOR_SIZE;
        object->alternate_sector_destination = (s32)gRuntime_pSharedWorkBase;
        object->sector_destination = (s32)gRuntime_pSharedWorkBase;
        object->transfer_mode = 1;
        break;

    case 4:
        object->phase_size = 5 * FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        object->alternate_sector_destination = (u32)gFile_pScreenBankBase;
        object->sector_destination = (u32)gFile_pScreenBankBase;
        object->transfer_mode = 1;
        break;
    }
}
