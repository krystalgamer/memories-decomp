#include "../types.h"
#include "model_record_tables.h"
#include "model_background.h"
#define MODEL_GRAPHICS_STATE_SCENE_BYTES
#include "model_graphics_state.h"

ModelBackgroundRecord *G32 gModel_pBackgroundRecord __attribute__((section(".sdata"))) =
    (ModelBackgroundRecord *)D_80091008;
u16 gModel_wProjectionDistance __attribute__((section(".sdata"))) = 0x012C;
s16 gModel_wBackgroundScrollX __attribute__((section(".sdata"))) = 0;
s16 gModel_wBackgroundOffsetY __attribute__((section(".sdata"))) = 0;
u16 gModel_wScreenYOverride __attribute__((section(".sdata"))) = 0;
u8 gModel_bSceneMode __attribute__((section(".sdata"))) = 0;
static u8 sModelGraphicsState_Pad95 __attribute__((section(".sdata"))) = 0;
u16 gModel_wPresentationBgmCommand __attribute__((section(".sdata"))) = 0;
u8 gModel_bOrbitDirectionTimer __attribute__((section(".sdata"))) = 0;
s8 gModel_bOrbitDirection __attribute__((section(".sdata"))) = 1;
s8 gModel_bPresentationPhase __attribute__((section(".sdata"))) = -1;
u8 gModel_bDrawMaskedPartsOnly __attribute__((section(".sdata"))) = 0;
s32 gModel_PartMaskAddress __attribute__((section(".sdata"))) = 0;
u8 gModel_bActiveSlotIndex __attribute__((section(".sdata"))) = 0;
u8 gModelDebug_bSelectedDigit __attribute__((section(".sdata"))) = 0;
u8 gGraphics_bPacketBufferIndex __attribute__((section(".sdata"))) = 0;
volatile u8 gGraphics_bFrameStep __attribute__((section(".sdata"))) = 0;
u8 D_8009AFA4[4] __attribute__((section(".sdata"))) = {1, 0, 0, 0};
static u16 sModelGraphicsState_ValueA8
    __attribute__((section(".sdata"))) = 1;
static u16 sModelGraphicsState_PadAA
    __attribute__((section(".sdata"))) = 0;
