#include "../../types.h"

/* SLES-03947 build of src/game/script_runtime.c, with its European bodies. */

#define VERSION_EUROPE

/* Values that differ in the European release; the US source names each
 * with an #ifndef default. */
/* One 50-sector menu-asset package per text language, from 0x2528. */
#define SCRIPT_DUEL_RESULT_MENU_ASSETS_START_SECTOR (0x2528 + gText_bLanguageIndex * 50)
#define SCRIPT_RUN_TICK_COMPLETION_MASK 0x2010
#define CAMPAIGN_SCENE_STAGE0_SECTORS 12
#define CAMPAIGN_SCENE_IMAGE_X 0x280
#define CAMPAIGN_SCENE_IMAGE_Y 0xE0
#define CAMPAIGN_SCENE_PACKAGE_START_SECTOR 0x23C9
#define CAMPAIGN_SCENE_PACKAGE_SECTOR_COUNT 0x2D

#include "../script_runtime.c"
