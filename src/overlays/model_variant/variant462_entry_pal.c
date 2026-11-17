#include "../../types.h"
/* The French header-462 images build the North American header-445 entry with
 * GCC 2.8.1 at the same size; only the palette loads from PAL x = 640. The
 * ribbons helper (variant445us_ribbons.c) is 12 bytes longer, so the sheets and
 * rings helpers start 12 and 16 bytes higher and the data 0x18 higher. */
#define VERSION_FRENCH
#define MODEL_VARIANT445US_CLUT_X 640
#include "../model_variant/variant445us_entry.c"
