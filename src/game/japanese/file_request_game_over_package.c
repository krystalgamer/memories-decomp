#include "../../types.h"

/* SLPM-86398 build of src/game/file_request_game_over_package.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * as is. */

/* Values that differ in the Japanese release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define GAME_OVER_PACKAGE_START_SECTOR 0x2152

#include "../file_request_game_over_package.c"
