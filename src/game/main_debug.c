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
    u8 flags = gMain_bActiveModeState;

    if ((flags & 0x40) == 0) {
        gMain_bActiveModeState = flags | 0x40;
        DebugMenu_Init();
    } else {
        DebugMenu_Update();
        if ((gMain_bActiveModeState & 0x40) == 0)
            gMain_bReturnMode = 0;
    }
}
