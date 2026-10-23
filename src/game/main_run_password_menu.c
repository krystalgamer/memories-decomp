#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#include "../types.h"
#include "../overlays/password/name_entry_keyboard.h"
#include "../overlays/password/shop.h"
#include "../psyq/rand.h"
#include "../psyq/setjmp.h"
#include "fade.h"
#include "file_transfer.h"
#include "main_menu_selection.h"
#include "game_over.h"
#include "main_modes.h"
#include "menu_record_reset.h"
#include "sound.h"
#include "main_services.h"
#include "options.h"
#include "../unmatched.h"
#include "main_mode_state.h"

void Main_RunPasswordMenu(void)
{
    unsigned char flags = gMain_bActiveModeState;

    if ((flags & 0x40) == 0) {
        gMain_bActiveModeState = flags | 0x40;
        File_RequestPasswordPackage();
        Password_InitShopScreen();
    }
    Password_UpdateShopScreen();
}
