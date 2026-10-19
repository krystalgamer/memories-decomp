#include "../../types.h"

/* SLES-03947 build of src/game/duel_field_card_render.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). Only Duel_RenderFieldCard3D
 * reads them. */
#define DUEL_FIELD_CARD_CLUT_Y 0xE1
#define DUEL_FIELD_CARD_CLUT_X16 0x28

#include "../duel_field_card_render.c"
