#include "../../types.h"

/* SLPM-86398 build of src/game/debug_menu_editor_entries.c: the symbols below
 * sit at other addresses in the Japanese executable and their US names are
 * taken there, so they are aliased (config/slpm_86398/symbols.txt has the
 * addresses). The US source is included as is. */
#define VERSION_JAPAN
#define D_8009B26C gJapanese_bActiveMainMode
#define gDebugMenu_wMessageId D_8009AE8C
#define gDebugMenu_wCampaignScene D_8009AE8E

/* Values that differ in the Japanese release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DEBUG_SCREEN_CONFIRM_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#define DEBUG_SCREEN_CANCEL_BUTTON PAD_BUTTON_CROSS

#include "../debug_menu_editor_entries.c"
