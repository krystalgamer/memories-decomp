#include "../../types.h"

/* SLES-03947 build of src/game/main_boot_frontend.c, with its European bodies
 * and the values that differ in the European release; the shared source names
 * each with an #ifndef default. */

#define VERSION_EUROPE
/* The European display height. */
#define GRAPHICS_DEFAULT_HEIGHT 256
#define DISPLAY_OBJECT_TRANSITION_HALF_HEIGHT 0x80

#include "../main_boot_frontend.c"
