#include "../types.h"

#include "fade.h"

extern u8 D_8009B145;
extern void Fade_InitOut(void);
extern void func_80015870(void);
extern void func_80012D4C(void);

void func_80015944(int color)
{
    FadeTransitionState *state;

    if (color == 0xFFFFFF) {
        D_8009B145 = 1;
    }
    *(s32 *)&D_800E9EC8 = color;
    Fade_InitOut();
    state = &D_800E9EC8;
    state->flags |= 0x30;
    func_80015870();
}

void func_80015998(void)
{
    FadeTransitionState *state = &D_800E9EC8;

    do {
        func_80012D4C();
    } while (state->flags & 0x80);
}
