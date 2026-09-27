#include "../../types.h"

/* SLES-03947 build of src/game/script_image_objects.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default. */
#define SCRIPT_IMAGE_ROW_X 0x280
#define SCRIPT_IMAGE_ROW_Y 0xD0
#define SCRIPT_IMAGE_TRANSFER_BASE 0x29E8

#include "../script_image_objects.c"
