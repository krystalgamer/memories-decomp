#include "../types.h"
#include "../overlays/main_menu/entrypoints.h"
#include "../overlays/password/name_entry_keyboard.h"
#include "../overlays/password/shop.h"
#include "../psyq/rand.h"
#include "../psyq/setjmp.h"
#include "fade.h"
#include "file_transfer.h"
#include "main_menu_selection.h"
#include "func_8003C2B4.h"
#include "game_over.h"
#include "main_modes.h"
#include "menu_record_reset.h"
#include "sound.h"
#include "main_services.h"
#include "options.h"
#define D_8009B26C_AS_SCALAR
#include "../unmatched.h"

void Main_RunPasswordMenu(void)
{
    unsigned char flags = D_8009B26C;

    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        File_RequestPasswordPackage();
        Password_InitShopScreen();
    }
    Password_UpdateShopScreen();
}
