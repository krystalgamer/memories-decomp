#ifndef YUGIOH_GAME_FADE_H
#define YUGIOH_GAME_FADE_H

#include "../ygo_types.h"

#define FADE_BAND_HEIGHT 8
#define FADE_SCREEN_WIDTH 320
#define FADE_SCREEN_HEIGHT (FADE_BAND_COUNT * FADE_BAND_HEIGHT)

extern FadeTransitionState gFade_State;
extern u8 D_800E9EC8_arr[FADE_TRANSITION_STATE_SIZE];

void func_800151B0(void);
void func_800151D8(void);
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
