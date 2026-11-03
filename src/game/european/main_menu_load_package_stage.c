#include "../../types.h"

u8 gFile_szSuMrgPath[] __attribute__((section(".rodata"))) =
    "M:/mrgSU/SU.mrg";

/* SLES-03947 build of src/game/main_menu_load_package_stage.c. */

#define VERSION_EUROPE

/* The package's first sector is gText_bLanguageIndex * 0x88 (gText_bLanguageIndex read
 * through $at). */
#define D_8009C02B_IN_DATA
#include "../duel_effect_resource_setup.h"
#define MAIN_MENU_PACKAGE_FIRST_SECTOR (gText_bLanguageIndex * 0x88)

#include "../main_menu_load_package_stage.c"
