#include "../../types.h"
/* The French images omit the work-size printf, so the data after the
 * shorter text and its rodata sit 0x24 lower. The
 * flashes cover the 256-line PAL screen. */
#define VERSION_FRENCH
#define GRAPHICS_DEFAULT_HEIGHT 256
#include "../model_variant/variant170_entry.c"
