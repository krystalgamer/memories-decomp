#include "../../types.h"
/* The French images build the header-432 entry with GCC 2.8.1 at the same
 * size; the four helpers keep their French C (variant449_sheet, _ribbons,
 * _streamers and _bands) at their French addresses, the data starts at
 * 0x2BB4 and the palette loads from PAL x = 640. */
#define VERSION_FRENCH
#define MODEL_VARIANT432_CLUT_X 640
#include "../model_variant/variant432_entry.c"
