#include "../../types.h"

/* SLES-03947 build of src/game/duel_draw_field_cards.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_FIELD_CARDS_CLUT_1 0x3829
#define DUEL_FIELD_CARDS_CLUT_3 0x3868

#include "../duel_draw_field_cards.c"
