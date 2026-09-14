#define D_8009B3EA_IN_DATA
#define D_8009B3ED_IN_DATA
#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_ARRAY
#include "../types.h"
#include "two_player_save_setup.h"
#include "mem_card.h"
#include "file_transfer.h"
#include "frontend_debug_state.h"
#include "../unmatched.h"
#include "debug_menu_two_player_entry.h"
#include "main_mode_state.h"

void DebugMenu_UpdateTwoPlayerDuelEntry(void)
{
    u8 flags = D_8009B2EB;
    s32 result;

    if ((flags & FRONTEND_STEP_FLAG_ENTERED) == 0) {
        D_8009B2EB = flags | FRONTEND_STEP_FLAG_ENTERED;
        D_8009B3ED = 0;
        D_8009B3EA = 0;
    }
    result = SaveData_UpdateDuelLoad();
    if (result != 0) {
        if (result == 1) {
            File_RequestMainMenuPackage();
            File_WaitForTransfers();
            D_8009B26C[0] = 0x10;
        }
        D_8009B2EB = 0;
    }
}

void DebugMenu_ResetEntryState(void)
{
    D_8009B2EB = 0;
}
