#include "../../types.h"

/* SLPM-86398 build of src/game/duel_update_card_pick_cursor.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_800907D8 gJapanese_D_800907D8
#define D_801A7AD8 gJapanese_DuelCardRecords

#define DUEL_VIEWER_CARD_ID_ADDRESS 0x8009B136
#define DUEL_VIEWER_Y_OFFSET_ADDRESS 0x8009B13B

#include "../duel_update_card_pick_cursor.c"
