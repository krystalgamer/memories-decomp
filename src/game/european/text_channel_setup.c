#include "../../types.h"

/* SLES-03947 to SLES-03951 build of src/game/text_channel_setup.c, with the
 * 100-byte European channel and no occupancy reset. */
#define VERSION_EUROPE
#include "../text_channel_setup.c"
