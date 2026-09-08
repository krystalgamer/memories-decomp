#include "../types.h"

#include "fade.h"

extern u8 D_8009B142;
extern u8 D_8009B143;
extern u8 D_8009B144;
extern u8 D_8009B145;
extern u8 D_8009B14A;
extern u8 D_8009B14B;
extern u8 D_8009B14C;

void func_8001572C(void)
{
    FadeTransitionState *state;
    int value;
    if (D_8009B145 != 0) {
        value = 0xFFFFFF;
        state = &gFade_State;
        *(s32 *)state = value;
        state->flags = 0x90;
        state->step = 0xC;
        D_8009B14C = 1;
        D_8009B144 = 1;
        D_8009B14B = 1;
        D_8009B143 = 1;
        D_8009B14A = 1;
        D_8009B142 = 1;
    }
}
