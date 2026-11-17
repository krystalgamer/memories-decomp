#include "../../types.h"

/* The North American header-406 ribbon reads its projected points through
 * pointer arithmetic and packed-word halves (see e589_ribbon_pal.c). */
#define MODEL_VARIANT423_RIBBON_PACKED_SCREEN
#include "../model_variant/e589_ribbon_pal.c"
