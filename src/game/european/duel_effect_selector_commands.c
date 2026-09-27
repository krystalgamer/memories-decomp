#include "../../types.h"

/* SLES-03947 build of src/game/duel_effect_command.c: only the functions enabled below. */

#define VERSION_EUROPE
#define VERSION_EUROPE_FUNC_80038024
#define VERSION_EUROPE_DUEL_EFFECT_FORWARD_SELECTOR
#define VERSION_EUROPE_DUEL_EFFECT_STREAM_SELECTOR

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_EFFECT_SELECTOR_FLAG 0x100

#include "../duel_effect_command.c"
