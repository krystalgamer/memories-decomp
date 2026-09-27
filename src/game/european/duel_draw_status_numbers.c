#include "../../types.h"

/* SLES-03947 build of src/game/duel_draw_status_numbers.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_STATUS_NUMBERS_CXCY 0xE10280

#include "../duel_draw_status_numbers.c"
