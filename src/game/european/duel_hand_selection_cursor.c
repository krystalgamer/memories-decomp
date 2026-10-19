#include "../../types.h"

/* SLES-03947 build of src/game/duel_hand_selection_cursor.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_HAND_STACK_INNER_Y 210
#define DUEL_HAND_STACK_COUNT_FIELD_5D 0xB0
#define DUEL_HAND_STACK_COUNT_CX 0x2D0

#include "../duel_hand_selection_cursor.c"
