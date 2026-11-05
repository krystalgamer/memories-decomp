#include "../types.h"
#include "model_record_tables.h"
#include "model_background.h"
#include "model_graphics_state.h"
#include "graphics_frame_controls.h"

ModelBackgroundRecord *gModel_pBackgroundRecord =
    (ModelBackgroundRecord *)gModel_abBackgroundRecords;
u16 gModel_wProjectionDistance = 0x012C;
s16 gModel_wBackgroundScrollX = 0;
s16 gModel_wBackgroundOffsetY = 0;
u16 gModel_wScreenYOverride = 0;
u8 gModel_bSceneMode = 0;
static u8 sModelGraphicsState_Pad95 = 0;
u16 gModel_wPresentationBgmCommand = 0;
u8 gModel_bOrbitDirectionTimer = 0;
s8 gModel_bOrbitDirection = 1;
s8 gModel_bPresentationPhase = -1;
u8 gModel_bDrawMaskedPartsOnly = 0;
s32 gModel_PartMaskAddress = 0;
u8 gModel_bActiveSlotIndex = 0;
#ifndef VERSION_JAPAN
u8 gModelDebug_bSelectedDigit = 0;
#endif
u8 gGraphics_bPacketBufferIndex = 0;
volatile u8 gGraphics_bFrameStep = 0;
u8 gGraphics_FrameStepControl = 1;
u8 D_8009AFA5 = 0;
u8 D_8009AFA6 = 0;
u8 D_8009AFA7 = 0;
#ifdef VERSION_JAPAN
/* The Japanese build has no selected-digit byte, so the four bytes above sit
 * unaligned at 0x8009AF0B; the rest of the block is single values. */
u8 D_8009AF0F_slpm_86398 = 1;
u8 D_8009AF10_slpm_86398 = 0;
u8 D_8009AF11_slpm_86398 = 0;
u16 D_8009AF12_slpm_86398 = 0;
#else
static u16 sModelGraphicsState_ValueA8
    = 1;
static u16 sModelGraphicsState_PadAA
    = 0;
#endif
