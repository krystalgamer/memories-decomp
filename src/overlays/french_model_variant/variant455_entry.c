#include "../../types.h"
/* The French images build the header-438 entry with GCC 2.8.1, 4 bytes
 * shorter, so the swarm helper (variant455_swarm.c) starts at 0x13B0 and
 * the data at 0x1978; the palette loads from PAL x = 640. */
#define VERSION_FRENCH
#define MODEL_VARIANT438_CLUT_X 640
#define func_8013C3B4 func_8013C3B0
#define D_8013C988 D_8013C978
#include "../model_variant/variant438_entry.c"
