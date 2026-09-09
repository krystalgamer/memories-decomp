#include "../types.h"

#include "fade.h"

void func_800151B0(void)
{
    FadeTransitionState *state = &gFade_State;

    state->flags = 0;
    state->level = 0;
    state->target_level = 0;
    state->step = 8;
    D_8009B145 = 0;
}
