#include "../../types.h"

/* SLES-03947 build of src/game/debug_menu_editor_entries.c. */
#define VERSION_EUROPE
#define DEBUG_MENU_CAMPAIGN_TEXT_BOX_HEIGHT 0x40

/* The value editor's tables sit at other addresses in the European
 * executable. */
#define gDebugMenu_awNibbleMasks D_8009BEC0

/* As in campaign_load_scene_package.c. */

/* func_80030808 reaches gText_bLanguageIndex through $at. */
#define D_8009C02B_IN_DATA

#include "../debug_menu_editor_entries.c"
