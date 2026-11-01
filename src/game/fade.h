#ifndef YUGIOH_GAME_FADE_H
#define YUGIOH_GAME_FADE_H

#include "../ygo_types.h"
#include "fade_constants.h"

#define FADE_BAND_HEIGHT 8
#define FADE_SCREEN_WIDTH 320
#define FADE_SCREEN_HEIGHT (FADE_BAND_COUNT * FADE_BAND_HEIGHT)

extern FadeTransitionState gFade_State;
#ifdef FADE_STATE_ARRAY_ALIAS
extern u8 D_800E9EC8_arr[FADE_TRANSITION_STATE_SIZE] asm("gFade_State");
#else
extern u8 D_800E9EC8_arr[FADE_TRANSITION_STATE_SIZE];
#endif
/* Byte 4 of gFade_State, the current fade level, under its own label.
   The updated level stays outside the small-data model so its split
   address can fill the final clamp branch delay slot. */
extern u8 gFade_abCurrentLevel[];

/* Latches the white-fade path until the transition completes. Most fade
   units reach it through small data; Script_OpShowImage defines the arm below
   to preserve its measured absolute-addressing form. */
#ifdef D_8009B145_IN_DATA
extern u8 D_8009B145 __attribute__((section(".data")));
#else
extern u8 D_8009B145;
#endif

/* The fade depth byte and the fade-active flag. Five of the fade family reach
 * both through $gp (small data) and take the plain arms below.
 *
 * build_deck_update.c and src/game/duel_effect_states.c address D_8009B140
 * with %hi/%lo instead, outside small data, so they take the .data arm the
 * same way script_image_commands.c takes one for D_8009B145. Both set it from
 * D_8009AF74[1] (display_object_helpers.h) and take that table's .data
 * arm for the same reason.
 *
 * graphics_frame.c reaches gFade_bOrderingTableMode with %hi/%lo as well and takes its
 * .data arm below; every other user keeps the plain small-data arm. */
#ifdef D_8009B140_IN_DATA
extern u8 D_8009B140 __attribute__((section(".data")));
#else
extern u8 D_8009B140;
#endif

#ifdef D_8009B141_IN_DATA
extern u8 gFade_bOrderingTableMode __attribute__((section(".data")));
#else
extern u8 gFade_bOrderingTableMode;
#endif

/* The colour a fade is heading for. Fade_Update copies the three into the
 * tint at gGraphics_bClearBlue/Green/Red (graphics_frame.h) in address order, with no
 * arithmetic on the way, so the roles follow the tint's: gFade_bTargetBlue is the
 * blue target, gFade_bTargetGreen green, gFade_bTargetRed red. Three writers and no
 * single producer -- Fade_InitInColor and Fade_ApplyWhiteLatchToFadeIn in the fade
 * family, and the start-up block in Main_InitHardware -- so the family
 * header holds the declaration.
 *
 * Main_InitHardware (src/game/main_services.c) takes the arm below and it
 * is a codegen input on that unit: retail stores the six D_8009B14x bytes
 * there through `lui $at` / `sb %lo(...)`, i.e. outside small data, and in
 * source order, which is what `.data` and `volatile` give. The three fade
 * units store them gp-relative and take the plain arm. */
#ifdef D_8009B14A_IN_DATA_VOLATILE
extern volatile u8 gFade_bTargetBlue __attribute__((section(".data")));
extern volatile u8 gFade_bTargetGreen __attribute__((section(".data")));
extern volatile u8 gFade_bTargetRed __attribute__((section(".data")));
#else
extern u8 gFade_bTargetBlue;
extern u8 gFade_bTargetGreen;
extern u8 gFade_bTargetRed;
#endif

void Fade_Init(void);
void Fade_StepBands(void);
void Fade_Update(FadeTransitionState *);
void Fade_DrawOverlay(void);
/* Fills FadeTransitionState::band_levels through the retail byte cursor. */
void Fade_FillBandLevels(s32);
/* Arms the white latch and leaves the fade complete at level 0 with a white
   clear colour and no ordering tables drawn: an instant cut to white. */
void Fade_CutToWhite(void);
void Fade_ApplyWhiteLatchToFadeIn(void);
void Fade_InitIn(void);
void Fade_StartIn(void);
void Fade_InitInColor(s32);
void Fade_ApplyWhiteLatchToFadeOut(void);
void Fade_InitOut(void);
void Fade_StartOut(void);
void Fade_InitOutColor(s32);
void Fade_Wait(void);
void Fade_WaitInitIn(void);
void Fade_WaitIn(void);
void Fade_WaitInitInColor(s32);
void Fade_WaitInitInKeepOverlay(void);
void Fade_WaitInitInKeepOverlayAndHideSecondaryTables(void);
void Fade_WaitInitOut(void);
void Fade_WaitOut(void);
void Fade_WaitInitOutColor(s32);
void Fade_WaitInitOutKeepOverlay(void);
void Fade_WaitInitOutKeepOverlayAndHideSecondaryTables(void);
void Fade_SetTargetLevel(s32, s32);
void Fade_SetLevel(s32);
void Fade_StartInKeepOverlay(void);
void Fade_StartInKeepOverlayAndHideSecondaryTables(void);
void Fade_StartOutKeepOverlay();
void Fade_StartOutKeepOverlayAndHideSecondaryTables(void);
void Fade_EnableOrderingTables(void);
void Fade_DisableOrderingTables(void);

#ifdef D_800E9ECE_AS_SCALAR
extern u8 D_800E9ECE;
#else
extern u8 D_800E9ECE[];
#endif
#ifdef D_800E9ECF_AS_SCALAR
extern u8 D_800E9ECF;
#else
extern u8 D_800E9ECF[];
#endif

#endif
