#include "../types.h"
#include "main_debug.h"
#include "main_modes.h"

extern u8 D_8009B269;
extern u8 D_8009B26C;

extern void Main_ResetFrontendRuntime(void);
extern void func_80030198(void);
extern void func_80031084(void);

void func_8002CDE8(void)
{
    Main_ResetFrontendRuntime();
}

void Main_RunDebugMenu(void)
{
    u8 flags = D_8009B26C;

    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        func_80030198();
    } else {
        func_80031084();
        if ((D_8009B26C & 0x40) == 0)
            D_8009B269 = 0;
    }
}
