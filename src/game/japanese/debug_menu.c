#include "../../types.h"

#define VERSION_JAPAN
#define DEBUG_SCREEN_CONFIRM_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#define DEBUG_SCREEN_CANCEL_BUTTON PAD_BUTTON_CROSS
/* Addressed directly, so the 0x60 folds into the %lo as retail has it. */
#define DEBUG_MENU_CANCEL_BUTTON PAD_BUTTON_CROSS
#define DEBUG_MENU_CONFIRM_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#include "../debug_menu.c"
