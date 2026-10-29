#include "../../types.h"
/* The French images build the header-380 entry with GCC 2.8.1, 4 bytes
 * shorter; the helpers after it start at 0xDE4, 0x1A9C (the French
 * variant397_sheets C) and 0x206C, the data at 0x2950, and the palette
 * loads from PAL x = 640. */
#define VERSION_FRENCH
#define MODEL_VARIANT380_CLUT_X 640
#define func_8013BDE8 func_8013BDE4
#define func_8013CAB8 func_8013CA9C
#define func_8013D08C func_8013D06C
#define D_8013D970 D_8013D950
#include "../model_variant/variant380_entry.c"
