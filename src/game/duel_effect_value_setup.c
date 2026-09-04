#include "../types.h"

extern s32 func_80036D3C(u8 *object);
extern s16 D_8009B27C __attribute__((section(".data")));
extern s16 D_8009B29C __attribute__((section(".data")));
extern s16 D_8009B2A8 __attribute__((section(".data")));
extern s16 D_8009B2AA __attribute__((section(".data")));
extern u8 D_8009B357;

void func_8003771C(u8 *object)
{
    s32 signed_value;
    s32 raw_value;

    object[0x51] = 0;
    D_8009B2A8 = func_80036D3C(object);
    D_8009B2AA = func_80036D3C(object);
    D_8009B29C = func_80036D3C(object);

    signed_value = D_8009B2AA;
    raw_value = (u16)D_8009B2AA;
    if (signed_value >= 0x1000) {
        D_8009B2AA = raw_value - 0x1000;
        object[0x51] = 10;
    }

    D_8009B357 = 7;
    D_8009B27C = 7;
}
