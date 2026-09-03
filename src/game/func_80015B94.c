#include "../types.h"

#include "fade.h"

extern void Fade_InitOut(void);
extern void func_80015870(void);
extern void func_80015998(void);

void func_80015B94(void)
{
    FadeTransitionState *state;

    Fade_InitOut();
    state = &D_800E9EC8;
    state->flags |= 6;
    func_80015870();
    func_80015998();
}
