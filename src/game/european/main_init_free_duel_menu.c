#include "../../types.h"

/* SLES-03947 build of src/game/main_init_free_duel_menu.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define FREE_DUEL_PACKAGE_SECTOR_COUNT 0x67
#define FREE_DUEL_PACKAGE_START_SECTOR 0x23F6

#include "../main_init_free_duel_menu.c"
