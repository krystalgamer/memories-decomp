#include "../../types.h"

/* SLPM-86398 build of src/game/script_runtime.c. */

#define VERSION_JAPAN

/* Values that differ in the Japanese release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define SCRIPT_DUEL_RESULT_MENU_ASSETS_START_SECTOR 0x1FB2
#define CAMPAIGN_SCENE_PACKAGE_START_SECTOR 0x1E62

#include "../script_runtime.c"
