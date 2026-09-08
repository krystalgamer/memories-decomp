#include "../types.h"

#include "fade.h"

extern u8 D_8009B145;
extern void func_80012D4C(void);

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
