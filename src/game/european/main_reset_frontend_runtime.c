#include "../../types.h"

/* SLES-03947 build of src/game/main_reset_frontend_runtime.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define INPUT_REPEAT_THRESHOLD 0x14
#define INPUT_REPEAT_RELOAD_VALUE 0x11

#include "../main_reset_frontend_runtime.c"
