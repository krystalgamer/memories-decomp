#include "../../types.h"
/* The French images omit the work-size printf and use the PAL screen
 * height, so the rodata and data after the shorter text sit lower. */
#define VERSION_FRENCH
#define GRAPHICS_DEFAULT_HEIGHT 256
#define D_8013C43C D_8013C428
#define D_8013C45C D_8013C438
#define D_8013C478 D_8013C454
#include "../model_variant/variant153us_entry.c"
