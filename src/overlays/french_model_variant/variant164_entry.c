#include "../../types.h"
/* The French images omit the work-size printf and flash all 256 PAL lines,
 * so the data after the shorter text and its rodata sit 0x1C lower. */
#define VERSION_FRENCH
#define D_8013E550 D_8013E534
#define D_8013E5DC D_8013E5C0
#include "../model_variant/variant147_entry.c"
