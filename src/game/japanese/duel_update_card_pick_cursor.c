#include "../../types.h"
#define VERSION_JAPAN

/* SLPM-86398 build of src/game/duel_update_card_pick_cursor.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */

#define DUEL_VIEWER_CARD_ID_ADDRESS 0x8009B136
#define DUEL_VIEWER_Y_OFFSET_ADDRESS 0x8009B13B

#include "../duel_update_card_pick_cursor.c"
