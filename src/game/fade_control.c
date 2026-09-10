#include "../types.h"
#include "main_frame.h"

#include "fade.h"

/* Fade_InitOutColor, the blocking Fade_Wait, and the wrappers built on them
   and on the other setup paths. Fade_InitOutColor and Fade_Wait need
   gcc_2_8_1_g8_split; the wrappers were recorded at gcc_2_8_1_g8 but
   compile to identical objects at gcc_2_8_1_g8_split, so the unit builds
   there. Fade_InitOut and Fade_StartOut, directly below, do not: fade_out.c
   changes without split addresses, and it stays its own unit between this
   one and the overlay and fade-in setup below it. */

void Fade_InitOutColor(int color)
{
    FadeTransitionState *state;

    if (color == 0xFFFFFF) {
        D_8009B145 = 1;
    }
    *(s32 *)&gFade_State = color;
    Fade_InitOut();
    state = &gFade_State;
    state->flags |= 0x30;
    func_80015870();
}

void Fade_Wait(void)
{
    FadeTransitionState *state = &gFade_State;

    do {
        func_80012D4C();
    } while (state->flags & 0x80);
}

void Fade_WaitInitIn(void)
{
    Fade_InitIn();
    Fade_Wait();
}

void Fade_WaitIn(void)
{
    Fade_StartIn();
    Fade_Wait();
}

void Fade_WaitInitInColor(s32 color)
{
    Fade_InitInColor(color);
    Fade_Wait();
}

void func_80015A50(void)
{
    FadeTransitionState *state;

    Fade_InitIn();
    state = &gFade_State;
    state->flags |= 2;
    func_8001572C();
    Fade_Wait();
}

void func_80015A94(void)
{
    FadeTransitionState *state;

    Fade_InitIn();
    state = &gFade_State;
    state->flags |= 6;
    func_8001572C();
    Fade_Wait();
}

void Fade_WaitInitOut(void)
{
    Fade_InitOut();
    Fade_Wait();
}

void Fade_WaitOut(void)
{
    Fade_StartOut();
    Fade_Wait();
}

void Fade_WaitInitOutColor(s32 color)
{
    Fade_InitOutColor(color);
    Fade_Wait();
}

void func_80015B50(void)
{
    FadeTransitionState *state;

    Fade_InitOut();
    state = &gFade_State;
    state->flags |= 2;
    func_80015870();
    Fade_Wait();
}

void func_80015B94(void)
{
    FadeTransitionState *state;

    Fade_InitOut();
    state = &gFade_State;
    state->flags |= 6;
    func_80015870();
    Fade_Wait();
}

void Fade_SetTargetLevel(s32 value, s32 flags)
{
    FadeTransitionState *state = &gFade_State;

    state->target_level = value;
    state->flags = flags | 0x80;
}

void Fade_SetLevel(s32 value)
{
    FadeTransitionState *state = &gFade_State;

    state->level = value;
    state->target_level = value;
    state->flags = 0x80;
}

void func_80015C0C(void)
{
    FadeTransitionState *state;

    Fade_InitIn();
    state = &gFade_State;
    state->flags |= 2;
    func_8001572C();
}

void func_80015C48(void)
{
    FadeTransitionState *state;

    Fade_InitIn();
    state = &gFade_State;
    state->flags |= 6;
    func_8001572C();
}

void func_80015C84(void)
{
    FadeTransitionState *state;

    Fade_InitOut();
    state = &gFade_State;
    state->flags |= 2;
    func_80015870();
}

void func_80015CC0(void)
{
    FadeTransitionState *state;

    Fade_InitOut();
    state = &gFade_State;
    state->flags |= 6;
    func_80015870();
}

void func_80015CFC(void)
{
    D_8009B141 = 1;
}

void func_80015D0C(void)
{
    D_8009B141 = 0;
}
