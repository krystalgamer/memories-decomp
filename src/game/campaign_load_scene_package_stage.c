#define DUEL_PACKAGE_STAGE_RAW_ARENAS
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "duel_load_package_stage.h"
#include "../unmatched.h"
#include "file_transfer_buffer.h"
#include "../ygo_types.h"
#include "campaign_scene_package.h"

#include "high_memory_addresses.h"
#include "shared_work_base.h"

/* Regional values, which the European wrapper defines for itself: the
   sectors stage 0 transfers (16 in the US build, 12 in the European one)
   and where stage 2 uploads its image row, (0x100, 0xF0) in the US build
   and (0x280, 0xE0) in the European one. */
#ifndef CAMPAIGN_SCENE_STAGE0_SECTORS
#define CAMPAIGN_SCENE_STAGE0_SECTORS 16
#endif
#ifndef CAMPAIGN_SCENE_IMAGE_X
#define CAMPAIGN_SCENE_IMAGE_X 0x100
#endif
#ifndef CAMPAIGN_SCENE_IMAGE_Y
#define CAMPAIGN_SCENE_IMAGE_Y 0xF0
#endif

void Campaign_LoadScenePackageStage(FileTransferDescriptor *p, s32 stage)
{
    s32 v_0;
    s32 v_1;
    s32 v_2;
    s32 v_3;
    s32 w;
    s32 t;
    s32 m;
    s32 c;

    switch (stage) {
    case 0:
        *(s16 *)&p->field_30.h.counter = 0x340;
        *(u16 *)&p->w = 0x40;
        m = 0xFFDDFFFF;
        v_0 = gFile_dwTransferState;
        *(u16 *)&p->h = 0x10;
        v_0 &= m;
        gFile_dwTransferState = v_0;
        w = gFile_dwTransferState;
        *(s16 *)&p->field_30.h.field_32 = 0;
        w |= 0x10000;
        gFile_dwTransferState = w;
        p->transfer_mode = 2;
        t = (p->sector_destination = gFile_TransferBufferBase);
        *(s32 *)&p->phase_size = CAMPAIGN_SCENE_STAGE0_SECTORS * FILE_SECTOR_SIZE;
        p->alternate_sector_destination = t + FILE_SECTOR_SIZE;
        break;

    case 1:
        *(s32 *)&p->phase_size = FILE_SECTOR_SIZE;
        m = 0xFFDCFFFF;
        v_1 = gFile_dwTransferState;
        t = gFile_TransferBufferBase;
        v_1 &= m;
        gFile_dwTransferState = v_1;
        p->alternate_sector_destination = t;
        p->sector_destination = t;
        p->transfer_mode = 1;
        break;

    case 2:
        c = gFile_TransferBufferBase;
        p->x = CAMPAIGN_SCENE_IMAGE_X;
        p->y = CAMPAIGN_SCENE_IMAGE_Y;
        p->w = 0x100;
        p->h = 1;
        LoadImage2((RECT *)p, (u32 *)c);
        *(s32 *)&p->alternate_sector_destination = (s32)D_801A8000;
        *(s32 *)&p->sector_destination = (s32)D_801A8000;
        m = 0xFFDCFFFF;
        v_2 = gFile_dwTransferState;
        p->phase_size = 0x1000;
        v_2 &= m;
        gFile_dwTransferState = v_2;
        p->transfer_mode = 1;
        break;

    case 3:
        *(s32 *)&p->phase_size = CAMPAIGN_DIALOG_PORTRAIT_PHASE_SIZE;
        m = 0xFFDCFFFF;
        v_3 = gFile_dwTransferState;
        t = (s32)gRuntime_pSharedWorkBase;
        v_3 &= m;
        gFile_dwTransferState = v_3;
        p->alternate_sector_destination = t;
        p->sector_destination = t;
        p->transfer_mode = 1;
        break;
    }
}
