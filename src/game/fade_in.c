#include "../types.h"

#include "fade.h"

void Fade_InitIn(void)
{
    FadeTransitionState *state = &gFade_State;

    state->target_level = 0xFF;
    state->flags = 0x80;
    D_8009B141 &= 0x7F;
    state->field_08 = 0;
    func_800156B8(state->level);
    state->step = 0xC;
    func_8001572C();
}

void Fade_StartIn(void)
{
    FadeTransitionState *state;

    Fade_InitIn();
    state = &gFade_State;
    state->step = 8;
    state->flags |= 1;
    func_8001572C();
}
