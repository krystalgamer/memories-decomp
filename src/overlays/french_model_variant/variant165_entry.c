#include "../../types.h"
/* The French images omit the work-size printf and its string and flash all
 * 256 PAL lines, so the data after the shorter text and its rodata sit 0x24
 * lower. */
#define VERSION_FRENCH
#define GRAPHICS_DEFAULT_HEIGHT 256
#define D_8013C728 D_8013C704
#define D_8013C744 D_8013C720
#include "../model_variant/variant148_entry.c"
