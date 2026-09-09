#include "../types.h"

#include "fade.h"

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
