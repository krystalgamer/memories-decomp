#include "../../types.h"
/* The French images build the header-412 entry with GCC 2.8.1, 4 bytes
 * shorter, so the two helpers (variant429_swarm.c, variant429_quads.c) start
 * 4 bytes lower and the data at 0x1D90. The palette loads from PAL x = 640. */
#define VERSION_FRENCH
#define MODEL_VARIANT412_CLUT_X 640
#define func_8013C340 func_8013C33C
#define func_8013C8B8 func_8013C8B4
#define D_8013CD88 D_8013CD90
#include "../model_variant/variant412_entry.c"
