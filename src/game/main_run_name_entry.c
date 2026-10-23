#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#include "../types.h"
#include "../overlays/password/name_entry_keyboard.h"
#include "file_transfer.h"
#include "main_modes.h"
#include "main_services.h"
#include "../unmatched.h"
#include "main_mode_state.h"

u8 gMain_bActiveModeState;

void Main_RunNameEntry(void)
{
    u8 flags = gMain_bActiveModeState;
    u8 value;

    if ((flags & 0x40) == 0) {
        gMain_bActiveModeState = flags | 0x40;
        File_RequestNameEntryPackage();
        NameEntry_Init();
    }
    if (NameEntry_PollCompletion()) {
        value = gMain_bReturnMode;
        gMain_bActiveModeState = value;
    }
}
