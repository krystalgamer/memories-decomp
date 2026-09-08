#include "../types.h"

#include "fade.h"

void Fade_InitOut(void)
{
    FadeTransitionState *state = &gFade_State;

    state->field_08 = 0xFF;
    state->target_level = 0;
    state->flags = 0x80;
    func_800156B8(state->level);
    state->step = 0xC;
    func_80015870();
}

void Fade_StartOut(void)
{
    FadeTransitionState *state;

    Fade_InitOut();
    state = &gFade_State;
    state->step = 8;
    state->flags |= 1;
    func_80015870();
}
