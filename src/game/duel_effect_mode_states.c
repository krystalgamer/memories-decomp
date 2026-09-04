#include "../types.h"

extern u8 D_8009B2EB;
extern u8 D_8009B254[];
extern int DuelEffect_UpdateState(void);

void func_80030E30(void)
{
    u8 flags = D_8009B2EB;

    if ((flags & 0x80) == 0) {
        D_8009B2EB = flags | 0x80;
        D_8009B254[0] = 3;
    }
    if (DuelEffect_UpdateState() == 0) {
        D_8009B2EB = 0;
    }
}

void func_80030E7C(void)
{
    u8 flags = D_8009B2EB;

    if ((flags & 0x80) == 0) {
        D_8009B2EB = flags | 0x80;
        D_8009B254[0] = 4;
    }
    if (DuelEffect_UpdateState() == 0) {
        D_8009B2EB = 0;
    }
}
