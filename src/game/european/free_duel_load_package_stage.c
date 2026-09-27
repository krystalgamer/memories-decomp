#include "../../types.h"

/* SLES-03947 build of src/game/free_duel_load_package_stage.c. The US build reaches
 * D_8009B0F4 through a second .data view named D_8009B0F4_abs; this build
 * names the object only. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default. */
#define FREE_DUEL_STAGE0_SECTORS 48
#define FREE_DUEL_IMAGE_X 0x280
#define FREE_DUEL_IMAGE_Y 0xD0

#define D_8009B0F4_abs D_8009B0F4

#include "../free_duel_load_package_stage.c"
