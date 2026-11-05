#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#include "../types.h"
#include "duel_interface_setup.h"
#include "debug_menu_update.h"
#include "main_debug.h"
#include "main_modes.h"
#include "main_reset_frontend_runtime.h"
#include "../unmatched.h"
#include "main_mode_state.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
u8 gMain_bReturnMode;
u8 gMain_bActiveModeState;

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
