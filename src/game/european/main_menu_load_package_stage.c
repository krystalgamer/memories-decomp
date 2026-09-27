#include "../../types.h"

/* SLES-03947 build of src/game/main_menu_load_package_stage.c. The US build reaches these objects
 * through second .data views named *_abs; this build names the
 * objects only. */
#define D_8009B0F4_abs D_8009B0F4

#define VERSION_EUROPE
#define VERSION_EUROPE_MAIN_MENU_LOAD_PACKAGE_STAGE
#define VERSION_EUROPE_FILE_REQUEST_MAIN_MENU_PACKAGE

/* The package's first sector is D_8009C02B * 0x88 (D_8009C02B read
 * through $at). */
#define D_8009C02B_IN_DATA
#include "../duel_effect_resource_setup.h"
#define MAIN_MENU_PACKAGE_FIRST_SECTOR (D_8009C02B * 0x88)

#include "../main_menu_load_package_stage.c"
