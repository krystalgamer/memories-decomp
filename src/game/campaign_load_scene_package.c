#include "../types.h"
#include "display_object.h"
#include "scene_script.h"
#include "file_transfer.h"
#define GRAPHICS_VIEWPORT_IN_DATA
#include "graphics_frame.h"
#include "image_transfer_rects.h"
#include "menu_record_reset.h"
#include "script_state.h"
#include "campaign_scene_package.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "campaign_create_primary_display_object.h"

#define HIGH_MEMORY_ADDRESSES_BASE_IN_DATA
#include "high_memory_addresses.h"
#include "shared_work_base.h"

/* The package's first sector in WA.MRG. A regional build defines its own. */
#ifndef CAMPAIGN_SCENE_PACKAGE_START_SECTOR
#define CAMPAIGN_SCENE_PACKAGE_START_SECTOR FILE_WA_CAMPAIGN_SCENE_START_SECTOR
#endif

#ifndef CAMPAIGN_SCENE_PACKAGE_SECTOR_COUNT
#define CAMPAIGN_SCENE_PACKAGE_SECTOR_COUNT FILE_WA_CAMPAIGN_SCENE_SECTOR_COUNT
#endif

#define SCENE_SCRIPT_SLOT_SIGNED_HEAD(slot) (*(s16 *)(slot))
#define CAMPAIGN_PACKAGE_WORDS(data) ((u32 *)(data))

/* Loads the fixed scene presentation/event package. The argument initializes
 * gScript_EventState for the event driver; it does not select a different disc page.
 * The 0x1E57/0x31 request uses Campaign_LoadScenePackageStage to route image,
 * palette, event script, and portrait data. After resetting viewport and slot
 * state, this function polls IsIdleGPU(10) until it returns zero, then uploads
 * 25 pairs from the arena held in gRuntime_pSharedWorkBase: 24-word by 48-row image
 * rectangles on a five-wide grid from x=0x380, followed by 64x1 CLUT
 * rectangles from y=0xF0.
 *
 * The clearing loop subscripts the record array rather than walking a
 * pointer. Both spellings give 112 instructions, but the walking pointer adds
 * a reference to `slot` and that changes which pseudo the allocator ranks
 * first: the %hi temporary for gSceneScript_Slots then takes $v0 and the address is
 * completed as `addiu $s0,$v0`, where retail coalesces the pair into
 * `lui $s0` / `addiu $s0,$s0` and emits it after the callback address rather
 * than before. Subscripting drops that reference and the prologue falls into
 * retail's order. */
void Campaign_LoadScenePackage(s16 arg0)
{
    SceneScriptSlot *slot;
    u32 *src;
    RECT *second;
    s32 i;

    slot = gSceneScript_Slots;
    gScript_EventState = arg0;
    gGraphics_sViewportY = 0;
    gGraphics_sViewportX = 0;
    gCampaign_PrimaryDisplayObject = 0;
    File_RequestAsyncTransfer(
        0, 0,
        CAMPAIGN_SCENE_PACKAGE_START_SECTOR,
        CAMPAIGN_SCENE_PACKAGE_SECTOR_COUNT,
        Campaign_LoadScenePackageStage, 0, 0
    );
    DisplayEffect_ResetMenuRecords();

    SCENE_SCRIPT_SLOT_SIGNED_HEAD(&slot[3]) = -1;
    for (i = 0; i < 3; i++) {
        slot[i].object_address = 0;
        slot[i].callback_control = 0;
    }
    File_WaitForTransfers();

    while (IsIdleGPU(10)) {
        ;
    }

    src = CAMPAIGN_PACKAGE_WORDS(gRuntime_pSharedWorkBase);
    gGraphics_aImageTransferRects[0].x = 0x380;
    gGraphics_aImageTransferRects[0].y = 0;
    gGraphics_aImageTransferRects[0].w = CAMPAIGN_DIALOG_PORTRAIT_IMAGE_WORD_WIDTH;
    gGraphics_aImageTransferRects[0].h = CAMPAIGN_DIALOG_PORTRAIT_IMAGE_HEIGHT;
    second = &gGraphics_aImageTransferRects[1];
    gGraphics_aImageTransferRects[1].x = 0x380;
    second->y = 0xF0;
    second->w = CAMPAIGN_DIALOG_PORTRAIT_CLUT_WORD_WIDTH;
    second->h = 1;

    i = 0;
    do {
        LoadImage2(&gGraphics_aImageTransferRects[0], src);
        LoadImage2(
            &gGraphics_aImageTransferRects[1],
            src + CAMPAIGN_DIALOG_PORTRAIT_IMAGE_SIZE / sizeof(u32)
        );
        i++;
        gGraphics_aImageTransferRects[1].y = gGraphics_aImageTransferRects[1].y + 1;
        gGraphics_aImageTransferRects[0].x =
            (i % CAMPAIGN_DIALOG_PORTRAIT_GRID_COLUMN_COUNT) *
                CAMPAIGN_DIALOG_PORTRAIT_IMAGE_WORD_WIDTH + 0x380;
        gGraphics_aImageTransferRects[0].y =
            (i / CAMPAIGN_DIALOG_PORTRAIT_GRID_COLUMN_COUNT) *
                CAMPAIGN_DIALOG_PORTRAIT_IMAGE_HEIGHT;
        if (gGraphics_aImageTransferRects[1].y >=
            0xF0 + CAMPAIGN_DIALOG_PORTRAIT_CLUT_ROWS_PER_COLUMN) {
            gGraphics_aImageTransferRects[1].x =
                gGraphics_aImageTransferRects[1].x +
                CAMPAIGN_DIALOG_PORTRAIT_CLUT_WORD_WIDTH;
            gGraphics_aImageTransferRects[1].y = 0xF0;
        }
        src += CAMPAIGN_DIALOG_PORTRAIT_RECORD_SIZE / sizeof(u32);
    } while (i < CAMPAIGN_DIALOG_PORTRAIT_COUNT);

    gCampaign_PrimaryDisplayObject = Campaign_CreatePrimaryDisplayObject();
}
