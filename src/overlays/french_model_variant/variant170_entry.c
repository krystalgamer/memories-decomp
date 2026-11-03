#include "../../types.h"
/* The French images omit the work-size printf and use the PAL screen
 * height, so the rodata and data after the shorter text sit lower; each
 * image binds the data names at its own addresses. */
#define VERSION_FRENCH
#define GRAPHICS_DEFAULT_HEIGHT 256
#include "../model_variant/variant153us_entry.c"
