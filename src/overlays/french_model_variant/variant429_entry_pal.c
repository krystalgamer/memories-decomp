#include "../../types.h"
/* The French images build the header-412 entry with GCC 2.8.1, 4 bytes
 * shorter, so the two helpers (variant429_swarm.c, variant429_quads.c) start
 * 4 bytes lower and the data at 0x1D90. The palette loads from PAL x = 640. */
#define VERSION_FRENCH
#define MODEL_VARIANT412_CLUT_X 640
#include "../model_variant/variant412_entry.c"
