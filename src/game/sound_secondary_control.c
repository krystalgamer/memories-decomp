#include "../types.h"
#include "sound.h"

extern void func_8004C77C(SDSecondaryState *);

int func_80049A64(void *input, short value)
{
    unsigned int tag;
    SDSecondaryState *state;
    D_8009B458->flag_0500 = 1;
    tag = *(unsigned int *)input;
    if (tag != 0x53455170 && tag != 0x6468544D &&
        tag != 0x2054444B && tag != 0x3154444B)
        return -1;
    state = D_8009B458;
    if (state->field_07E0 == -1) {
        state->field_07E8 = input;
        state->field_07E0 = value;
        state->field_07E2 = 2;
        state->flag_0500 = 0;
        return 0;
    }
    state->flag_0500 = 0;
    return -1;
}

void func_80049AF4(s32 arg0)
{
    register s32 one asm("$17") = 1;

    D_8009B458->flag_0500 = one;

    if (D_8009B458->field_07E0 == -1) {
        D_8009B458->flag_0500 = 0;
        return;
    }

    D_8009B458->field_07EC = 0x10000;
    D_8009B458->field_07DC = D_8009B458->field_07E8;
    func_8004C77C(D_8009B458);

    if ((arg0 & 0xFF) == 0) {
        D_8009B458->field_07E2 = 4;
    } else {
        D_8009B458->flag_0502 = one;
        D_8009B458->field_07E2 = 1;
    }

    D_8009B458->flag_0500 = 0;
}
