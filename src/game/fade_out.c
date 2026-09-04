#include "../types.h"

#include "fade.h"

extern void func_800156B8(u8);
extern void func_80015870(void);

void Fade_InitOut(void)
{
    FadeTransitionState *state = &D_800E9EC8;

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
    state = &D_800E9EC8;
    state->step = 8;
    state->flags |= 1;
    func_80015870();
}
