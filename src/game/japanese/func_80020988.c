#include "../../types.h"

/* SLPM-86398 build of src/game/func_80020988.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * as is. */

/* Values that differ in the Japanese release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define FUNC_80020988_CANCEL_BUTTON PAD_BUTTON_CROSS

#include "../func_80020988.c"
