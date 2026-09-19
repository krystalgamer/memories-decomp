#define D_8009B118_IN_DATA
#include "../types.h"
#include "display_asset_banks.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#define D_800101D8_IN_DATA
#include "file_transfer.h"
#include "campaign_map_load_package_stage.h"
#include "campaign_flags.h"
#include "file_constants.h"
#include "../unmatched.h"

#define HIGH_MEMORY_ADDRESSES_BASE_IN_DATA
#include "high_memory_addresses.h"

/* The Campaign Map package load. File_RequestEgyptOverworldPackage queues
   the Egypt overworld package with CampaignMap_LoadPackageStage as its stage
   callback, starting one package length further on the disc once
   CAMPAIGN_FLAG_TOURNAMENT_COMPLETE is set. */

void CampaignMap_LoadPackageStage(FileTransferDescriptor *object, s32 mode) {
    switch (mode) {
    case 0:
        object->phase_size = 6 * FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->value_0C = (u32)D_800101D8;
        object->value_08 = (u32)D_800101D8;
        object->done = 1;
        break;

    case 1:
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->phase_size = 134 * FILE_SECTOR_SIZE;
        object->value_0C = (s32)D_80010000;
        object->value_08 = (s32)D_80010000;
        object->done = 1;
        break;

    case 2:
        object->value_0C = (s32)D_801AF000;
        object->value_08 = (s32)D_801AF000;
        object->phase_size = FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->done = 1;
        break;

    case 3:
        object->field_30.h.counter = 0x1C0;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4_abs &= 0xFFDDFFFF;
        D_8009B0F4_abs |= 0x10000;
        object->done = 2;
        object->phase_size = 16 * FILE_SECTOR_SIZE;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + FILE_SECTOR_SIZE;
        break;

    case 4:
        object->phase_size = FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->value_0C = D_8009B118;
        object->value_08 = D_8009B118;
        object->done = 1;
        break;

    case 5:
        object->x = 0x100;
        object->y = 0xF0;
        object->w = 0x100;
        object->h = 4;
        LoadImage2((RECT *)object, (u32 *)D_8009B118);
        break;
    }
}

void File_RequestEgyptOverworldPackage(void){int x=0;if(Campaign_TestStoryFlag(CAMPAIGN_FLAG_TOURNAMENT_COMPLETE))x=FILE_WA_EGYPT_OVERWORLD_SECTOR_COUNT;File_RequestAsyncTransfer(0,0,x+FILE_WA_EGYPT_OVERWORLD_START_SECTOR,FILE_WA_EGYPT_OVERWORLD_SECTOR_COUNT,CampaignMap_LoadPackageStage,0,0);File_WaitForTransfers();}
