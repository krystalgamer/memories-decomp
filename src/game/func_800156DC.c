#include "../types.h"

#include "fade.h"

extern u8 D_8009B142;
extern u8 D_8009B143;
extern u8 D_8009B144;
extern u8 D_8009B145;
extern void Fade_InitOut(void);
extern void func_80015D0C(void);

void func_800156DC(void)
{
    FadeTransitionState *state;
    D_8009B145 = 1;
    Fade_InitOut();
    state = &D_800E9EC8;
    state->flags = 0;
    state->level = 0;
    D_8009B142 = 0xFF;
    D_8009B143 = 0xFF;
    D_8009B144 = 0xFF;
    func_80015D0C();
}
