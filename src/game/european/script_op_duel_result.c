#include "../../types.h"

/* SLES-03947 build of src/game/script_op_duel_result.c, with its European bodies. */

#define VERSION_EUROPE
#define D_8009C02B_IN_DATA
/* One 50-sector menu-asset package per text language, from 0x2528. */
#define SCRIPT_DUEL_RESULT_MENU_ASSETS_START_SECTOR (0x2528 + D_8009C02B * 50)
/* This build names the loader words directly. */
#define D_8009B0F4_abs D_8009B0F4
#define D_8009B134_abs D_8009B134
/* As in campaign_load_scene_package.c. */
#define D_8009B2A0 D_8009C1F4

#include "../script_op_duel_result.c"
