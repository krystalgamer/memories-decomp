#include "../../types.h"

/* SLPM-86398 build of src/game/password_load_package_stage.c. */

/* Values that differ in the Japanese release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define PASSWORD_PACKAGE_START_SECTOR 0x1F3A

#include "../password_load_package_stage.c"
