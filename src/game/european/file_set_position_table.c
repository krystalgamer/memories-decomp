#include "../../types.h"

/* SLES-03947 build of src/game/file_set_position_table.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define FILE_POSITION_TABLE_Y 0xE0

#include "../file_set_position_table.c"
