#include "../types.h"

#include "fade.h"

extern void func_80015780(void);
extern void func_8001572C(void);
extern void func_80015998(void);

void func_80015A50(void)
{
    FadeTransitionState *state;

    func_80015780();
    state = &D_800E9EC8;
    state->flags |= 2;
    func_8001572C();
    func_80015998();
}

void func_80015A94(void)
{
    FadeTransitionState *state;

    func_80015780();
    state = &D_800E9EC8;
    state->flags |= 6;
    func_8001572C();
    func_80015998();
}
