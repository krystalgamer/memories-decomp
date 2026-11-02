#include "../../types.h"
#include "duel_effect_channel.h"

#define VERSION_JAPAN
/* Addressed directly, so the 0x60 folds into the %lo as retail has it. */
#define DEBUG_MENU_SECOND_BOX(boxes) \
    ((DuelEffectChannel *)&((JapaneseDuelEffectChannel *)gText_aChannels)[1])
#define DEBUG_MENU_CANCEL_BUTTON PAD_BUTTON_CROSS
#define DEBUG_MENU_CONFIRM_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#include "../debug_menu_update.c"
