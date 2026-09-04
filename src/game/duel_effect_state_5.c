#include "../types.h"

extern u16 D_8009B2AA[];
/* Separate linker names keep GCC from retaining these addresses across calls. */
extern u16 Base2_8009B2AA[];
extern u16 D_8009B2A8[];
extern u16 Base2_8009B2A8[];
extern u16 D_8009B270[];
extern u16 D_8009B27C[];
extern u8 D_8009B357;

extern s32 func_80036D3C(u8 *);

void func_8003767C(u8 *state)
{
    s32 result;

    D_8009B2AA[0] = 0;
    D_8009B2A8[0] = 0;
    result = func_80036D3C(state);
    D_8009B270[0] = result;

    if (result & 0x8000) {
        u8 **slot = (u8 **)(state + *(s8 *)(state + 0x58) * 4);
        u8 *script = *slot;
        s32 value = *script;

        *slot = script + 1;
        Base2_8009B2AA[0] = value;
        Base2_8009B2A8[0] = func_80036D3C(state);
    }

    D_8009B357 = 5;
    D_8009B27C[0] = 5;
    state[0x51] = 10;
}
