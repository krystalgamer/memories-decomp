#include "../types.h"

#include "fade.h"

extern u8 D_8009B141;
extern void func_800156B8(u8);
extern void func_8001572C(void);

void func_80015780(void)
{
    FadeTransitionState *state = &D_800E9EC8;

    state->target_level = 0xFF;
    state->flags = 0x80;
    D_8009B141 &= 0x7F;
    state->field_08 = 0;
    func_800156B8(state->level);
    state->step = 0xC;
    func_8001572C();
}

void func_800157DC(void)
{
    FadeTransitionState *state;

    func_80015780();
    state = &D_800E9EC8;
    state->step = 8;
    state->flags |= 1;
    func_8001572C();
}
