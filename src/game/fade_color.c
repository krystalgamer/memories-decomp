#include "../types.h"

#include "fade.h"

extern u8 D_8009B14A;
extern u8 D_8009B14B;
extern u8 D_8009B14C;

void Fade_InitInColor(int color)
{
    FadeTransitionState *state;

    if (color == 0xFFFFFF) {
        D_8009B145 = 1;
    }
    *(s32 *)&gFade_State = color;
    Fade_InitIn();
    state = &gFade_State;
    state->flags |= 0x30;
    func_8001572C();
}

void func_80015870(void)
{
    FadeTransitionState *state;
    int color;

    if (D_8009B145 != 0) {
        color = 0xFFFFFF;
        state = &gFade_State;
        *(s32 *)state = color;
        state->flags = 0xB0;
        state->step = 0xC;
        D_8009B14A = 0xFF;
        D_8009B14B = 0xFF;
        D_8009B14C = 0xFF;
    }
}
