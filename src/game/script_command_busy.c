#include "../types.h"

extern u16 D_8009B27C;

s32 func_8002E3B4(void)
{
    u16 value = D_8009B27C;

    if (!(value & 0x8000)) {
        D_8009B27C = value | 0x8000;
        return 0;
    }
    return 1;
}

void func_8002E3DC(void)
{
    func_8002E3B4();
}
