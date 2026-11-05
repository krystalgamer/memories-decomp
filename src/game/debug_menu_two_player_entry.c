#define D_8009B3EA_IN_DATA
#define D_8009B3ED_IN_DATA
#include "../types.h"
#include "two_player_save_setup.h"
#include "mem_card.h"
#include "file_transfer.h"
#include "frontend_debug_state.h"
#include "../unmatched.h"
#include "debug_menu_two_player_entry.h"
#include "main_mode_state.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
u8 gDebugMenu_bStep;

void DebugMenu_UpdateTwoPlayerDuelEntry(void)
{
    u8 flags = gDebugMenu_bStep;
    s32 result;

    if ((flags & FRONTEND_STEP_FLAG_ENTERED) == 0) {
        gDebugMenu_bStep = flags | FRONTEND_STEP_FLAG_ENTERED;
        gSaveData_PairSetupState[0] = 0;
        gSaveData_LoadPairState[0] = 0;
    }
    result = SaveData_UpdateDuelLoad();
    if (result != 0) {
        if (result == 1) {
            File_RequestMainMenuPackage();
            File_WaitForTransfers();
            gMain_bActiveModeState[0] = 0x10;
        }
        gDebugMenu_bStep = 0;
    }
}

void DebugMenu_ResetEntryState(void)
{
    gDebugMenu_bStep = 0;
}
