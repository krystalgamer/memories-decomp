#include "../../types.h"
/* The French images omit the work-size printf, so the data after the
 * shorter text and its rodata sit 0x24 lower. The
 * flashes cover the 256-line PAL screen. */
#define VERSION_FRENCH
#define GRAPHICS_DEFAULT_HEIGHT 256
#define D_8013CFE4 D_8013CFC0
#define D_8013D038 D_8013D014
#include "../model_variant/variant170_entry.c"
