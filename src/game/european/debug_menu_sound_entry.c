#include "../../types.h"

/* SLES-03947 build of src/game/debug_menu_editor_entries.c: only the functions enabled below. */

#define VERSION_EUROPE
#define VERSION_EUROPE_FUNC_80030808
#define VERSION_EUROPE_DEBUG_MENU_UPDATE_SOUND_ENTRY
#define VERSION_EUROPE_DEBUG_MENU_UPDATE_CAMPAIGN_ENTRY

#define DEBUG_MENU_CAMPAIGN_TEXT_BOX_HEIGHT 0x40

/* As in campaign_load_scene_package.c. */
#define D_8009B2A0 D_8009C1F4

/* func_80030808 reaches D_8009C02B through $at. */
#define D_8009C02B_IN_DATA

#include "../debug_menu_editor_entries.c"
