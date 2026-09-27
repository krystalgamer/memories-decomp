#include "../../types.h"

/* SLES-03947 build of src/game/script_image_objects.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default. */
#define SCRIPT_IMAGE_SCREEN_X 0x280
#define SCRIPT_IMAGE_SCREEN_Y 0xD0
#define SCRIPT_IMAGE_TRANSFER_BASE 0x29E8

/* ScriptImage_CreateObject takes the splat name of its European address,
 * as in script_image_rebuild's wrapper. */
#define ScriptImage_CreateObject func_8002E10C

#include "../script_image_objects.c"
