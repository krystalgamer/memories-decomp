#include "../../types.h"
/* The French tint grid covers the 256-line PAL screen, so its tile heights
 * become shifts and the data after the shorter text sits 0x20 lower; each
 * image binds the data names at its own addresses. */
#define GRAPHICS_DEFAULT_HEIGHT 256
#include "../model_variant/variant826_entry.c"
