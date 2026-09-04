#include "../types.h"

#include "fade.h"

extern u8 D_8009B145;
extern u8 D_8009B14A;
extern u8 D_8009B14B;
extern u8 D_8009B14C;
extern void func_80015780(void);
extern void func_8001572C(void);

void func_8001581C(int color)
{
    FadeTransitionState *state;

    if (color == 0xFFFFFF) {
        D_8009B145 = 1;
    }
    *(s32 *)&D_800E9EC8 = color;
    func_80015780();
    state = &D_800E9EC8;
    state->flags |= 0x30;
    func_8001572C();
}

void func_80015870(void)
{
    FadeTransitionState *state;
    int color;

    if (D_8009B145 != 0) {
        color = 0xFFFFFF;
        state = &D_800E9EC8;
        *(s32 *)state = color;
        state->flags = 0xB0;
        state->step = 0xC;
        D_8009B14A = 0xFF;
        D_8009B14B = 0xFF;
        D_8009B14C = 0xFF;
    }
}
