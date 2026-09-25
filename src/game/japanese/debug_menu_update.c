#include "../../types.h"
#include "duel_effect_channel.h"

#define VERSION_JAPAN
/* Addressed directly, so the 0x60 folds into the %lo as retail has it. */
#define DEBUG_MENU_SECOND_BOX(boxes) \
    ((DuelEffectChannel *)&((JapaneseDuelEffectChannel *)D_800EB0F8)[1])
#define DEBUG_MENU_CANCEL_BUTTON PAD_BUTTON_CROSS
#define DEBUG_MENU_CONFIRM_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#define FntPrint func_8007DB74
#define D_8009AF54 gJapanese_D_8009AF54
#define D_800EB0F8 gJapanese_D_800EB0F8
#define gDebugMenu_bPage gJapanese_DebugMenuPage
#define gDebugMenu_apfnAlternatePageSteps D_80090C2C
#define gDebugMenu_apfnPrimaryPageSteps D_80090C34
#include "../debug_menu_update.c"
