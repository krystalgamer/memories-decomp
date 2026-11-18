#include "../../types.h"

/* The North American header-424 spiral tests its arm and polygon pointers
 * before storing the even-phase size (see h424_spiral_pal.c). */
#define MODEL_VARIANT441_SPIRAL_GUARDED_SIZE
#include "../model_variant/h424_spiral_pal.c"
