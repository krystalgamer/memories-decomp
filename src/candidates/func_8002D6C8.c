/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8 this
 * source rebuilt the target byte for byte, but only by
 * 1 inline asm statement, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/main_run_frontend_menus.c.
 */
#include "../types.h"
#include "../overlays/main_menu/entrypoints.h"
#include "../overlays/password/name_entry_keyboard.h"
#include "../overlays/password/shop.h"
#include "../psyq/rand.h"
#include "../psyq/setjmp.h"
#include "../game/fade.h"
#include "../game/file_transfer.h"
#include "../game/main_menu_selection.h"
#include "../game/func_8003C2B4.h"
#include "../game/game_over.h"
#include "../game/main_modes.h"
#include "../game/menu_record_reset.h"
#include "../game/sound.h"
#include "../game/main_services.h"
#include "../game/options.h"
#define D_8009B26C_AS_SCALAR
#include "../unmatched.h"

void Main_RunOptionsMenu(void)
{
    unsigned char flags = D_8009B26C;

    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        func_8003C2B4();
        Options_Init();
        Fade_WaitIn();
    }
    if (Options_Update() == 0) {
        unsigned char value;

        SD_BGMFadeOut();
        value = D_8009B269;
        __asm__ volatile("nop");
        D_8009B26C = value;
    }
}
