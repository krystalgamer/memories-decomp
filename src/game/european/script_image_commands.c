#include "../../types.h"

/* SLES-03947 build of src/game/script_image_commands.c. */
#define VERSION_EUROPE
/* Values that differ in the European release; the US source names each
 * with an #ifndef default. */
#define SCRIPT_IMAGE_SCREEN_X 0x280
#define SCRIPT_IMAGE_SCREEN_Y 0xD0
/* The US build reaches these objects through second .data views named
 * *_abs; this build names the objects only. */
#include "../script_image_commands.c"
