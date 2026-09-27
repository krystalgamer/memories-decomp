#include "../../types.h"

/* SLES-03947 build of src/game/script_run_tick.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define SCRIPT_RUN_TICK_COMPLETION_MASK 0x2010

#include "../script_run_tick.c"
