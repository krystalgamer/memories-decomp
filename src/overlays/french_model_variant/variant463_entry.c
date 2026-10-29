#include "../../types.h"
/* The French images build the header-446 entry with GCC 2.8.1 at the same
 * size; the helpers after it move (0x2318, 0x32E8), the uncalled rings
 * helper keeps its French C (variant463_rings.c) at 0x37D4, the data starts
 * at 0x3C08 and the palette loads from PAL x = 640. */
#define VERSION_FRENCH
#define MODEL_VARIANT446_CLUT_X 640
#define func_8013D314 func_8013D318
#define func_8013E2D8 func_8013E2E8
#define D_8013EBF4 D_8013EC08
#include "../model_variant/variant446_entry.c"
