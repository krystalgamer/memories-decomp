#include "../../types.h"

/* The North American header-424 spiral tests its arm and polygon pointers
 * before storing the even-phase size (see variant441_spiral.c). */
#define MODEL_VARIANT441_SPIRAL_GUARDED_SIZE
#include "../spanish_model_variant/variant441_spiral.c"
