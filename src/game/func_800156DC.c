#include "../types.h"
#include "graphics_frame.h"

#include "fade.h"

void func_800156DC(void)
{
    FadeTransitionState *state;
    D_8009B145 = 1;
    Fade_InitOut();
    state = &gFade_State;
    state->flags = 0;
    state->level = 0;
    D_8009B142 = 0xFF;
    D_8009B143 = 0xFF;
    D_8009B144 = 0xFF;
    func_80015D0C();
}
