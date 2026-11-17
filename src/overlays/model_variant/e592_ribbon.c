#include "../../types.h"

/* The North American header-409 ribbon helper reads its projected points
 * through pointer arithmetic (see e592_ribbon_pal.c). */
#define MODEL_VARIANT409_RIBBON_POINTER_SCREEN
#include "../model_variant/e592_ribbon_pal.c"
