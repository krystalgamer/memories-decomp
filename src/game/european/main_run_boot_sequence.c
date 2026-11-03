#include "../../types.h"

/* SLES-03947 build of src/game/main_run_boot_sequence.c, with its European bodies. */

#define VERSION_EUROPE
#define TEXT_LANGUAGE_INDEX_IN_DATA
/* The European display height. */
#define GRAPHICS_DEFAULT_HEIGHT 256
/* As in main_init.c. */

#include "../main_run_boot_sequence.c"
