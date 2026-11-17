#include "../../types.h"
/* The French images build the header-380 entry with GCC 2.8.1, 4 bytes
 * shorter; the helpers after it start at 0xDE4, 0x1A9C (the French
 * variant397_sheets C) and 0x206C, the data at 0x2950, and the palette
 * loads from PAL x = 640. */
#define VERSION_FRENCH
#define MODEL_VARIANT380_CLUT_X 640
#include "../model_variant/variant380_entry.c"
