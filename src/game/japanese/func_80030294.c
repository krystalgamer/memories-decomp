#include "../../types.h"

/* SLPM-86398 build of src/game/func_80030294.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * as is. */
#define D_8009AF4C gJapanese_D_8009AF4C
#define D_8009AF54 gJapanese_D_8009AF54

/* Values that differ in the Japanese release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DEBUG_SCREEN_CONFIRM_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#define DEBUG_SCREEN_CANCEL_BUTTON PAD_BUTTON_CROSS

#include "../func_80030294.c"
