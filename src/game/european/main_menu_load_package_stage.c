#include "../../types.h"

/* SLES-03947 build of src/game/main_menu_load_package_stage.c. */

#define VERSION_EUROPE

/* The package's first sector is gText_bLanguageIndex * 0x88 (gText_bLanguageIndex read
 * through $at). */
#define MAIN_MENU_PACKAGE_FIRST_SECTOR (gText_bLanguageIndex * 0x88)

#include "../card_preview.h"
#include "../main_menu_load_package_stage.c"
