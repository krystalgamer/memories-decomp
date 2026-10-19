#include "../../types.h"

/* SLES-03947 build of src/game/debug_menu_editor_entries.c. */
#define VERSION_EUROPE
#define DEBUG_MENU_CAMPAIGN_TEXT_BOX_HEIGHT 0x40

/* The value editor's tables sit at other addresses in the European
 * executable. */
#define D_8009AF4C D_8009BEC0
#define D_8009AF54 D_8009BEC8
#define D_8009AF58 D_8009BECC
#define D_800EAED8 D_800EBE40

/* As in campaign_load_scene_package.c. */
#define D_8009B2A0 D_8009C1F4

/* func_80030808 reaches D_8009C02B through $at. */
#define D_8009C02B_IN_DATA

#include "../debug_menu_editor_entries.c"
