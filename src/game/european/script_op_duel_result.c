#include "../../types.h"

/* SLES-03947 build of src/game/script_op_duel_result.c, with its European bodies. */

#define VERSION_EUROPE
#define D_8009C02B_IN_DATA
/* One 50-sector menu-asset package per text language, from 0x2528. */
#define SCRIPT_DUEL_RESULT_MENU_ASSETS_START_SECTOR (0x2528 + gText_bLanguageIndex * 50)
/* This build names the loader words directly. */
/* As in campaign_load_scene_package.c. */

#include "../script_op_duel_result.c"
