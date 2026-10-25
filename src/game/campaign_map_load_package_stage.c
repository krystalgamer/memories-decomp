#define D_8009B118_IN_DATA
#include "../types.h"
#include "display_asset_banks.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#define D_800101D8_IN_DATA
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "campaign_map_load_package_stage.h"
#include "campaign_flags.h"
#include "file_constants.h"
#include "../unmatched.h"
#include "display_resource_banks.h"
#include "file_transfer_buffer.h"

#define HIGH_MEMORY_ADDRESSES_BASE_IN_DATA
#include "high_memory_addresses.h"
#include "shared_work_base.h"

/* Where case 5 uploads its image row: (0x100, 0xF0) in the US build and
   (0x280, 0xE0) in the European one, whose wrapper defines its own. */
#ifndef CAMPAIGN_MAP_IMAGE_X
#define CAMPAIGN_MAP_IMAGE_X 0x100
#endif
#ifndef CAMPAIGN_MAP_IMAGE_Y
#define CAMPAIGN_MAP_IMAGE_Y 0xF0
#endif

/* The package's first sector in WA.MRG. A regional build defines its own. */
#ifndef CAMPAIGN_MAP_PACKAGE_START_SECTOR
#define CAMPAIGN_MAP_PACKAGE_START_SECTOR FILE_WA_EGYPT_OVERWORLD_START_SECTOR
#endif

/* The Campaign Map package load. File_RequestEgyptOverworldPackage queues
   the Egypt overworld package with CampaignMap_LoadPackageStage as its stage
   callback, starting one package length further on the disc once
   CAMPAIGN_FLAG_TOURNAMENT_COMPLETE is set. */

void CampaignMap_LoadPackageStage(FileTransferDescriptor *object, s32 mode) {
    switch (mode) {
    case 0:
        object->phase_size = 6 * FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        object->alternate_sector_destination = (u32)gFile_pScreenBankBase;
        object->sector_destination = (u32)gFile_pScreenBankBase;
        object->transfer_mode = 1;
        break;

    case 1:
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        object->phase_size = 134 * FILE_SECTOR_SIZE;
        object->alternate_sector_destination = (s32)gRuntime_pSharedWorkBase;
        object->sector_destination = (s32)gRuntime_pSharedWorkBase;
        object->transfer_mode = 1;
        break;

    case 2:
        object->alternate_sector_destination = (s32)gDisplay_abResourceBank0;
        object->sector_destination = (s32)gDisplay_abResourceBank0;
        object->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        object->transfer_mode = 1;
        break;

    case 3:
        object->field_30.h.counter = 0x1C0;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        gFile_dwTransferState_abs &= 0xFFDDFFFF;
        gFile_dwTransferState_abs |= 0x10000;
        object->transfer_mode = 2;
        object->phase_size = 16 * FILE_SECTOR_SIZE;
        object->sector_destination = gFile_TransferBufferBase;
        object->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 4:
        object->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        object->alternate_sector_destination = gFile_TransferBufferBase;
        object->sector_destination = gFile_TransferBufferBase;
        object->transfer_mode = 1;
        break;

    case 5:
        object->x = CAMPAIGN_MAP_IMAGE_X;
        object->y = CAMPAIGN_MAP_IMAGE_Y;
        object->w = 0x100;
        object->h = 4;
        LoadImage2((RECT *)object, (u32 *)gFile_TransferBufferBase);
        break;
    }
}

void File_RequestEgyptOverworldPackage(void){int x=0;if(Campaign_TestStoryFlag(CAMPAIGN_FLAG_TOURNAMENT_COMPLETE))x=FILE_WA_EGYPT_OVERWORLD_SECTOR_COUNT;File_RequestAsyncTransfer(0,0,x+CAMPAIGN_MAP_PACKAGE_START_SECTOR,FILE_WA_EGYPT_OVERWORLD_SECTOR_COUNT,CampaignMap_LoadPackageStage,0,0);File_WaitForTransfers();}
