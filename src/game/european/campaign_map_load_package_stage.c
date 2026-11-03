#include "../../types.h"

/* SLES-03947 build of src/game/campaign_map_load_package_stage.c. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default. */
#define CAMPAIGN_MAP_IMAGE_X 0x280
#define CAMPAIGN_MAP_IMAGE_Y 0xE0
#define CAMPAIGN_MAP_PACKAGE_START_SECTOR 0x2622

#include "../campaign_map_load_package_stage.c"
