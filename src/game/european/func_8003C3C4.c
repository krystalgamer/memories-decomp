#include "../../types.h"

#define VERSION_EUROPE
#include "../display_asset_banks.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../file_transfer.h"
#include "../duel_effect_resource_setup.h"

/* The second earlier PAL frontend package callback: like the first, with a
   48-sector opening phase.
   It is the four-mode Options_LoadPackageStage body (frontend_package_stages.c)
   with its own constants. No caller or address reference is known in the
   European executables. */
void Frontend_LoadPackageStageVariantB(FileTransferDescriptor *object, s32 mode)
{
    switch (mode) {
    case 0:
        object->field_30.h.field_32 = 0x100;
        object->field_30.h.counter = 0;
        object->w = 0x40;
        object->h = 0x10;
        gFile_dwTransferState &= 0xFFDDFFFF;
        object->phase_size = 48 * FILE_SECTOR_SIZE;
        gFile_dwTransferState |= 0x10000;
        object->transfer_mode = 2;
        object->sector_destination = gFile_TransferBufferBase;
        object->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 1:
        object->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->alternate_sector_destination = gFile_TransferBufferBase;
        object->sector_destination = gFile_TransferBufferBase;
        object->transfer_mode = 1;
        break;

    case 2:
        object->x = 0x280;
        object->y = 0xE0;
        object->w = 0x100;
        object->h = 4;
        LoadImage2((RECT *)object, (u32 *)gFile_TransferBufferBase);
        object->alternate_sector_destination = (s32)gDisplay_abResourceBank0;
        object->sector_destination = (s32)gDisplay_abResourceBank0;
        object->phase_size = 2 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->transfer_mode = 1;
        break;

    case 3:
        object->phase_size = 6 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->alternate_sector_destination = (s32)gFile_pScreenBankBase;
        object->sector_destination = (s32)gFile_pScreenBankBase;
        object->transfer_mode = 1;
        break;
    }
}
