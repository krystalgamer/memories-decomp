#include "../../types.h"

/* The North American header-410 ribbons helper reads its projected points
 * through pointer arithmetic (see h410_ribbons_pal.c). */
#define MODEL_VARIANT410_RIBBONS_POINTER_SCREEN
#include "../model_variant/h410_ribbons_pal.c"
