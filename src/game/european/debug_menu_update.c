#include "../../types.h"

#define VERSION_EUROPE
#define DEBUG_MENU_SECOND_BOX(boxes) (&D_800EB0F8[1])
#define gDebugMenu_apfnAlternatePageSteps gEuropean_DebugMenuAlternatePageSteps
#define gDebugMenu_apfnPrimaryPageSteps gEuropean_DebugMenuPrimaryPageSteps

#include "../debug_menu_update.c"
