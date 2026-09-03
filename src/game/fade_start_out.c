#include "../types.h"

#include "fade.h"

extern void Fade_InitOut(void);
extern void func_80015870(void);

void Fade_StartOut(void)
{
    FadeTransitionState *state;

    Fade_InitOut();
    state = &D_800E9EC8;
    state->step = 8;
    state->flags |= 1;
    func_80015870();
}
