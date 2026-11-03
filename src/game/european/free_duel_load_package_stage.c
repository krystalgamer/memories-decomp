#include "../../types.h"

/* SLES-03947 build of src/game/free_duel_load_package_stage.c. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default. */
#define FREE_DUEL_STAGE0_SECTORS 48
#define FREE_DUEL_IMAGE_X 0x280
#define FREE_DUEL_IMAGE_Y 0xD0

#include "../free_duel_load_package_stage.c"
