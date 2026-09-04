#include "../types.h"

#include "fade.h"

extern void func_80015780(void);
extern void func_8001572C(void);

void func_80015C0C(void)
{
    FadeTransitionState *state;

    func_80015780();
    state = &D_800E9EC8;
    state->flags |= 2;
    func_8001572C();
}

void func_80015C48(void)
{
    FadeTransitionState *state;

    func_80015780();
    state = &D_800E9EC8;
    state->flags |= 6;
    func_8001572C();
}
