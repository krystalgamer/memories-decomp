#include "../../types.h"

/* SLES-03947 build of src/game/duel_effect_command.c. */
#define VERSION_EUROPE
/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_EFFECT_SELECTOR_FLAG 0x100
#include "../duel_effect_command.c"
