#include "../../types.h"
#define DISPLAY_OBJECT_SPRITE_TEXTURE_U(texture) (((texture) & 0xF0) + 0x280)
#define DISPLAY_OBJECT_SPRITE_TEXTURE_V_BASE(texture) \
    ((((texture) & 0x300) >> 4) + 0xD0)

/* SLES-03947 to SLES-03951 build of src/game/display_object_runtime.c. */
#define VERSION_EUROPE
#define SPRITE_SHEET_SCREEN_HEIGHT 256

/* SLES-03947 build of src/game/display_object_helpers.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DISPLAY_OBJECT_DIVP_PIV 288

#include "../display_object_system.c"
