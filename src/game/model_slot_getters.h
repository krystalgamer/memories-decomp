#ifndef MEMORIES_DECOMP_MODEL_SLOT_GETTERS_H
#define MEMORIES_DECOMP_MODEL_SLOT_GETTERS_H
#include "../types.h"

/* Returns the frame step: the volatile counter at gGraphics_bFrameStep clamped to at
 * most 6. Callers scale an animation rate by it (m->playback_speed * step) or test
 * it directly against 2, so the result is consumed as a signed value even
 * though the definition computes it in an unsigned register. */
s32 Model_GetFrameStep(void);

/* Returns 2 while the load phase differs from 0xFF, otherwise the active
 * byte's nonzero test. Load requests, transfer completion and
 * Model_AdvanceSlotLoad establish the phase sequence; this is not a Boolean
 * predicate. Slot lookup, int ABI and the phase-before-active read stay exact. */
int Model_GetSlotReadiness(int index);

/* Shared model/graphics state at 0x8009AF88-0x8009AFAB.
 *
 * The model scene owns the pointer and model state through gModelDebug_bSelectedDigit.
 * Graphics_BeginFrame and Graphics_SyncFrame publish the active buffer and
 * bounded frame step in gGraphics_bPacketBufferIndex-gGraphics_FrameStepControl.
 * gGraphics_bFrameStep is volatile, as model.c defines it; Graphics_SyncFrame's
 * store and Model_GetFrameStep's two reads are non-volatile in retail, so
 * those three accesses cast at the use. */
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
extern u8 gGraphics_bPacketBufferIndex;
extern volatile u8 gGraphics_bFrameStep;

/* A4-A7 are four separate bytes (graphics_frame_controls.h): the frame-step
 * override gGraphics_FrameStepControl, unclassified D_8009AFA5, D_8009AFA6
 * (returned by func_80058DC0) and gModel_bCreditsActiveSlot, the credits' active slot (0/1). */

s32 Model_GetActiveSlotIndex(void);

/* Four reads of one model slot, each selected by slot index rather than by a
 * slot pointer, so a caller needs nothing of the record's layout.
 *
 * Model_GetSlotAnimationFrame returns the slot's 12.4 fixed-point animation
 * position shifted right by four.
 *
 * Model_GetSlotAnimationLength takes two steps: it reads the animation index
 * at 0xBF5 that Model_GetSlotAnimationIndex also returns, then uses it to
 * index animation_duration_rows and hands back that row's `max`. Its return type is `u16`
 * because `max` is one (model.h:44); model.h:39-40 lists it among the four
 * functions that read the same halfword.
 *
 * Model_GetSlotAnimationSpeed, Model_GetSlotAnimationFrame and
 * Model_GetSlotAnimationLength have no caller yet -- none in C and none in the
 * generated assembly. They are declared here anyway so that whoever
 * decompiles their callers finds a signature instead of inventing one. */
s32 Model_GetSlotAnimationSpeed(s32 index);
s32 Model_GetSlotAnimationIndex(s32 index);
s32 Model_GetSlotAnimationFrame(s32 index);
u16 Model_GetSlotAnimationLength(s32 idx);

#endif
