#include "../../types.h"

/* SLES-03947 build of src/game/script_image_commands.c: only the functions enabled below. */

#define VERSION_EUROPE
#define VERSION_EUROPE_SCRIPT_OP_SHOW_IMAGE

/* Values that differ in the European release; the US source names each
 * with an #ifndef default. */
#define SCRIPT_IMAGE_SCREEN_X 0x280
#define SCRIPT_IMAGE_SCREEN_Y 0xD0

/* The US build reaches these objects through second .data views named
 * *_abs; this build names the objects only. */
#define D_8009B0F4_abs D_8009B0F4
#define D_8009B134_abs D_8009B134

#include "../script_image_commands.c"
