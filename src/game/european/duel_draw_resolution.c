#include "../../types.h"

/* SLES-03947 build of src/game/duel_draw_resolution.c: the symbols below sit at other addresses in the
 * European executable and their US names are taken there, so they are aliased
 * (config/sles_03947/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B23A gDuel_wSceneStateFlags

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_DRAW_RESOLUTION_CARD_Y 0xA2
#define DUEL_CARD_READBACK_HEIGHT 0xD4

#include "../duel_draw_resolution.c"
#include "../duel_field_card_render_data.h"
