#include "../types.h"

#include "fade.h"

void func_80015BD8(s32 value, s32 flags)
{
    FadeTransitionState *state = &D_800E9EC8;

    state->target_level = value;
    state->flags = flags | 0x80;
}

void func_80015BF0(s32 value)
{
    FadeTransitionState *state = &D_800E9EC8;

    state->level = value;
    state->target_level = value;
    state->flags = 0x80;
}
