#include "../../types.h"

#define VERSION_EUROPE
#define D_8009AF54 D_8009BEC8
#define DEBUG_MENU_SECOND_BOX(boxes) (&D_800EB0F8[1])
#define gDebugMenu_apfnAlternatePageSteps gEuropean_DebugMenuAlternatePageSteps
#define gDebugMenu_apfnPrimaryPageSteps gEuropean_DebugMenuPrimaryPageSteps

#include "../debug_menu_update.c"
