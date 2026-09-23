#include "../../types.h"

/* SLPM-86398 build of src/game/dialog_update_choice.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * as is. */

/* Values that differ in the Japanese release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DIALOG_UPDATE_CHOICE_CONFIRM_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)

#include "../dialog_update_choice.c"
