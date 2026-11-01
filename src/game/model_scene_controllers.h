#ifndef MEMORIES_DECOMP_MODEL_SCENE_CONTROLLERS_H
#define MEMORIES_DECOMP_MODEL_SCENE_CONTROLLERS_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "model.h"

/* Steps one of two VRAM copy animations, gated on gModel_bAuxiliaryActive[0] and selected
 * by byte 0xA0 of the selected background record at gModel_pBackgroundRecord (the intro
 * controller switches on the same byte, values 0 to 6). Model_UpdateScene
 * calls it every scene update, directly after the background renderer
 * func_8004DE24. Both arms build the same four-halfword RECT
 * at gModel_ImageCopyRect and hand it to MoveImage; they differ in the source region and
 * in how the frame is derived from VSync(-1) -- case 1 cycles three 16-pixel
 * columns, case 4 indexes gModel_abImageCopyFrameRows. Any other selector
 * value does nothing.
 *
 * gModel_pBackgroundRecord comes from model_graphics_state.h; the four
 * rectangle halfwords now use their native SDK type here, while the separate
 * animation gate retains its independent native label.
 *
 * Model_UpdateScene in model_scene_states.c is the only caller and held the only
 * declaration. */
void Model_StepBackgroundVramAnimation(void);
extern u8 gModel_bAuxiliaryActive[];
extern RECT gModel_ImageCopyRect;
/* Separate field aliases preserve the original gp-relative store schedule;
 * using the RECT base for every member makes GCC retain the y-field address
 * and synthesize the base before MoveImage. */
extern s16 gModel_ImageCopyRectY;
extern s16 gModel_ImageCopyRectW;
extern s16 gModel_ImageCopyRectH;

extern u8 gModel_szAnimationSoundTraceFormat[];

/* Model_UpdateAnimatedBattle's per-slot action: plays animation
 * guardian_star_selection + 3 when it has frames, moves the camera towards the other slot
 * for two thirds of that length (at least 60, or the record's override), and
 * sets gModel_bCameraHoldState from the record's flag bits. */
void Model_StartBattleSlotAction(s32 index);
void Model_PlaySlotAnimationSounds(s32 index);

/* Last selected model IDs (including slot 2's stage) and the two model flags
 * mirrored by Model_SetSlotProperties and edited by
 * ModelDebug_UpdateController. */
extern s16 gModel_awSlotModelIds[MODEL_SLOT_COUNT];
extern u8 gModelDebug_abSlotAfFlags[2];
extern u8 gModelDebug_abSlotGfFlags[2];
extern s16 D_800F2B4A[];

/* Resets the model scene: clears the two counters at gModel_wBackgroundScrollX/gModel_wBackgroundOffsetY and
 * the packed state words, and rebuilds the coordinate unit. Three units call it
 * to enter a scene; Main_RunCredits also calls it before starting its scene. */
void Model_ResetSceneView(void);

/* Sets one model slot's properties. Variadic on purpose, and the arity is the
 * argument: slot 2 takes one property, slots 0 and 1 take five, and a first
 * property below zero leaves the current one alone. The definition reads them
 * with va_arg in that order, so a caller passing the wrong count for its slot
 * reads whatever follows on the stack. */
void Model_SetSlotProperties(s32 idx, ...);

/* Clears the scene's 256-entry table to VRAM, resets the three transfer
 * channels and the model slot flags, and drops the scene counters. Like
 * Model_ResetSceneView above it is also called from Main_RunCredits through
 * this declaration. */
void Model_ResetSceneState(void);

#endif
