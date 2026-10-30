#include "../../types.h"

/* The North American header-424 spiral tests its arm and polygon pointers
 * before storing the even-phase size (see variant441_spiral.c). */
#define MODEL_VARIANT441_SPIRAL_GUARDED_SIZE
#define func_8013E630 func_8013E640
#include "../spanish_model_variant/variant441_spiral.c"
