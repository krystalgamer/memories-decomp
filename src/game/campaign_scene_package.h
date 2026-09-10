#ifndef MEMORIES_DECOMP_CAMPAIGN_SCENE_PACKAGE_H
#define MEMORIES_DECOMP_CAMPAIGN_SCENE_PACKAGE_H

#include "../types.h"
#include "../ygo_types.h"
#include "display_object.h"

#define CAMPAIGN_DIALOG_PORTRAIT_COUNT 25
#define CAMPAIGN_DIALOG_PORTRAIT_GRID_COLUMN_COUNT 5
#define CAMPAIGN_DIALOG_PORTRAIT_FIRST_EFFECT_ID 0x41
#define CAMPAIGN_DIALOG_PORTRAIT_IMAGE_WIDTH 48
#define CAMPAIGN_DIALOG_PORTRAIT_IMAGE_HEIGHT 48
#define CAMPAIGN_DIALOG_PORTRAIT_IMAGE_WORD_WIDTH \
    (CAMPAIGN_DIALOG_PORTRAIT_IMAGE_WIDTH / 2)
#define CAMPAIGN_DIALOG_PORTRAIT_IMAGE_SIZE 0x900
#define CAMPAIGN_DIALOG_PORTRAIT_CLUT_WORD_WIDTH 0x40
#define CAMPAIGN_DIALOG_PORTRAIT_CLUT_SIZE 0x80
#define CAMPAIGN_DIALOG_PORTRAIT_RECORD_SIZE \
    (CAMPAIGN_DIALOG_PORTRAIT_IMAGE_SIZE + \
     CAMPAIGN_DIALOG_PORTRAIT_CLUT_SIZE)
#define CAMPAIGN_DIALOG_PORTRAIT_CLUT_ROWS_PER_COLUMN 16
#define CAMPAIGN_DIALOG_PORTRAIT_PHASE_SIZE 0xF000
#define CAMPAIGN_DIALOG_PORTRAIT_USED_SIZE \
    (CAMPAIGN_DIALOG_PORTRAIT_COUNT * \
     CAMPAIGN_DIALOG_PORTRAIT_RECORD_SIZE)
#define CAMPAIGN_DIALOG_PORTRAIT_SPILL_SIZE \
    (CAMPAIGN_DIALOG_PORTRAIT_PHASE_SIZE - \
     CAMPAIGN_DIALOG_PORTRAIT_USED_SIZE)

typedef char CampaignDialogPortrait_record_size_must_be_0x980[
    CAMPAIGN_DIALOG_PORTRAIT_RECORD_SIZE == 0x980 ? 1 : -1
];
typedef char CampaignDialogPortrait_spill_size_must_be_0x280[
    CAMPAIGN_DIALOG_PORTRAIT_SPILL_SIZE == 0x280 ? 1 : -1
];

/* Initializes the fixed scene presentation package: seeds the event-script
 * flag word D_8009B2A4 from its argument, clears the viewport origin, and
 * requests the package transfer.
 *
 * Both callers declared the parameter s32 while the definition takes s16,
 * and nothing compared them: this unit had no header. Neither call is
 * affected -- func_80031354 passes a constant 0, and Main_RunCampaign passes
 * the u8 gCampaignSceneIndex, which widens the same way to either type -- so
 * the header states the definition's own width.
 *
 * func_80030998 also reaches this function, but through a hand-written
 * relocation in inline assembly rather than a C call, so no declaration
 * governs that site. */
void Campaign_LoadScenePackage(s16 arg0);
void Campaign_LoadScenePackageStage(
    FileTransferDescriptor *descriptor, s32 stage
);

/* The scene package's primary display object. Campaign_LoadScenePackage
 * creates it, func_8002F630 hides and restores it around the duel-result
 * screen, and the inline-assembly event driver updates the same slot. */
extern DisplayObject *D_8009B2A0;

#endif
