#ifndef YUGIOH_GAME_FADE_H
#define YUGIOH_GAME_FADE_H

#include "../ygo_types.h"

#define FADE_BAND_HEIGHT 8
#define FADE_SCREEN_WIDTH 320
#define FADE_SCREEN_HEIGHT (FADE_BAND_COUNT * FADE_BAND_HEIGHT)

extern FadeTransitionState gFade_State;
extern u8 D_800E9EC8_arr[FADE_TRANSITION_STATE_SIZE];

/* Latches the white-fade path until the transition completes. Most fade
   units reach it through small data; Script_OpShowImage defines the arm below
   to preserve its measured absolute-addressing form. */
#ifdef D_8009B145_IN_DATA
extern u8 D_8009B145 __attribute__((section(".data")));
#else
extern u8 D_8009B145;
#endif

/* The colour a fade is heading for. Fade_Update copies the three into the
 * tint at D_8009B142/143/144 (graphics_frame.h) in address order, with no
 * arithmetic on the way, so the roles follow the tint's: D_8009B14A is the
 * blue target, D_8009B14B green, D_8009B14C red. Three writers and no
 * single producer -- Fade_InitInColor and func_8001572C in the fade
 * family, and the start-up block in main_services.c -- so the family
 * header holds the declaration.
 *
 * main_services.c takes the arm below and it is a codegen input on that
 * unit: retail stores the six D_8009B14x bytes there through `lui $at` /
 * `sb %lo(...)`, i.e. outside small data, and in source order, which is
 * what `.data` and `volatile` give. The three fade units store them
 * gp-relative and take the plain arm. */
#ifdef D_8009B14A_IN_DATA_VOLATILE
extern volatile u8 D_8009B14A __attribute__((section(".data")));
extern volatile u8 D_8009B14B __attribute__((section(".data")));
extern volatile u8 D_8009B14C __attribute__((section(".data")));
#else
extern u8 D_8009B14A;
extern u8 D_8009B14B;
extern u8 D_8009B14C;
#endif

void func_800151B0(void);
void Fade_StepBands(void);
void Fade_Update(u8 *);
void Fade_DrawOverlay(void);
void func_800156B8(s32);
void func_800156DC(void);
void func_8001572C(void);
void Fade_InitIn(void);
void Fade_StartIn(void);
void Fade_InitInColor(s32);
void func_80015870(void);
void Fade_InitOut(void);
void Fade_StartOut(void);
void Fade_InitOutColor(s32);
void Fade_Wait(void);
void func_800159D8(void);
void func_80015A00(void);
void func_80015A28(s32);
void func_80015A50(void);
void func_80015A94(void);
void func_80015AD8(void);
void Fade_WaitOut(void);
void func_80015B28(s32);
void func_80015B50(void);
void func_80015B94(void);
void func_80015BD8(s32, s32);
void func_80015BF0(s32);
void func_80015C0C(void);
void func_80015C48(void);
void func_80015C84();
void func_80015CC0(void);
void func_80015CFC(void);
void func_80015D0C(void);

#endif
