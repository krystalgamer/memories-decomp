#include "../../types.h"

/* The North American header-446 ribbons helper reads its projected points
 * through pointer arithmetic (see h446_ribbons_pal.c). */
#define MODEL_VARIANT446_RIBBONS_POINTER_SCREEN
#include "../model_variant/h446_ribbons_pal.c"
