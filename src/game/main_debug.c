#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#include "../types.h"
#include "duel_interface_setup.h"
#include "debug_menu_update.h"
#include "main_debug.h"
#include "main_modes.h"
#include "main_reset_frontend_runtime.h"
#include "../unmatched.h"
#include "main_mode_state.h"

void Main_PrepareFrontendLoop(void)
{
    Main_ResetFrontendRuntime();
}

void Main_RunDebugMenu(void)
{
    u8 flags = D_8009B26C;

    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        DebugMenu_Init();
    } else {
        DebugMenu_Update();
        if ((D_8009B26C & 0x40) == 0)
            D_8009B269 = 0;
    }
}
