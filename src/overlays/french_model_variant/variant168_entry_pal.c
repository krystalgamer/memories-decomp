#include "../../types.h"
/* The French images omit the work-size printf and use the PAL screen
 * height, so the rodata and data after the shorter text sit 0x24 lower. */
#define VERSION_FRENCH
#define GRAPHICS_DEFAULT_HEIGHT 256
#include "../model_variant/variant151_entry.c"
