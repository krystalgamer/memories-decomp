#include "../types.h"

#include "fade.h"
#include "fade_overlay_controls.h"

void Fade_Init(void)
{
    FadeTransitionState *state = &gFade_State;

    state->flags = 0;
    state->level = 0;
    state->target_level = 0;
    state->step = 8;
    gFade_bWhiteLatch = 0;
}
