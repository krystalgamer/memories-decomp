#ifndef MEMORIES_DECOMP_MODEL_GRAPHICS_STATE_H
#define MEMORIES_DECOMP_MODEL_GRAPHICS_STATE_H

#include "../types.h"
#include "model_get_frame_step.h"

/* Shared model/graphics state at 0x8009AF88-0x8009AFAB.
 *
 * The model scene owns the pointer and model state through gModelDebug_bSelectedDigit.
 * Graphics_BeginFrame and Graphics_SyncFrame publish the active buffer and
 * bounded frame step in gGraphics_bPacketBufferIndex-gGraphics_FrameStepControl.
 *
 * graphics_frame.c needs the absolute-address declaration arm below. Its
 * retail stores use %hi/%lo and its view of gGraphics_bFrameStep is non-volatile.
 * Model_GetFrameStep also uses a non-volatile view, but keeps two explicit
 * source reads. The C owner remains volatile and in .sdata for the default
 * view. */
/* Pointer to the active 0xB2-byte model record. Model_SetSlotProperties
 * selects it from gModel_abBackgroundRecords; model view/update code reads fields through it.
 * The background renderer reads through +0xB1, including texture metadata,
 * and reads this pointer before checking its model slot's active byte.
 * The complete record belongs to model_background.h; only byte-backed
 * table selection crosses into the record view. */
struct ModelBackgroundRecord;
extern struct ModelBackgroundRecord *gModel_pBackgroundRecord;

/* Model view and scene state. gModel_wBackgroundScrollX/gModel_wBackgroundOffsetY are the background
 * horizontal scroll and vertical offset that Model_UpdateBackgroundScroll
 * derives from the view yaw and pitch; gModel_bSceneMode is the unsigned scene
 * mode; gModel_bPresentationPhase is the signed phase-completion sentinel. */
extern u16 gModel_wProjectionDistance;
extern s16 gModel_wBackgroundScrollX;
extern s16 gModel_wBackgroundOffsetY;
/* Signed model screen-Y override; 0x8000 selects the slot-derived default. */
extern u16 gModel_wScreenYOverride;
extern u8 gModel_bSceneMode;
/* BGM command replayed by the model presentation's opening phase. */
extern u16 gModel_wPresentationBgmCommand;
extern u8 gModel_bOrbitDirectionTimer;
/* Its definition is `s8 gModel_bOrbitDirection = 1;` (model_graphics_state.c:14), the
 * two assembly readers load it `lb` and none `lbu`, and both writers store
 * -1: Model_LockOrbitDirection (matched) and Model_SeparateViewFromSlots. */
extern s8 gModel_bOrbitDirection;
extern s8 gModel_bPresentationPhase;

/* Out-of-band arguments Model_UpdateTintRequests publishes for Model_DrawSlot:
 * a one-byte flag and a word-sized address expressed as s32 in retail C. */
extern u8 gModel_bDrawMaskedPartsOnly;
extern s32 gModel_PartMaskAddress;
/* Model slot whose handler is currently running. */
extern u8 gModel_bActiveSlotIndex;
extern u8 gModelDebug_bSelectedDigit;

/* Active graphics buffer, bounded frame step, and frame-step override. */
#ifdef MODEL_GRAPHICS_STATE_FRAME_ABSOLUTE
extern u8 gGraphics_bPacketBufferIndex __attribute__((section(".data")));
extern u8 gGraphics_bFrameStep __attribute__((section(".data")));
#elif defined(MODEL_GRAPHICS_STATE_CLAMP_NONVOLATILE)
extern u8 gGraphics_bPacketBufferIndex;
extern u8 gGraphics_bFrameStep;
#else
extern u8 gGraphics_bPacketBufferIndex;
extern volatile u8 gGraphics_bFrameStep;
#endif

/* gGraphics_FrameStepControl (graphics_frame_controls.h) is the four-byte
 * object at A4-A7: frame-step override, unclassified byte, the byte
 * func_80058DC0 returns, and the scene's active slot (0/1). Its consumers
 * reach the later bytes as elements of that one object. */

s32 Model_GetActiveSlotIndex(void);

#endif
