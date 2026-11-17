#include "../../types.h"

/* The North American header-371 strips read the second strip's next
 * column through the current column pointer (see e547_strips_pal.c). */
#define MODEL_VARIANT388_STRIPS_SHARED_COLUMN
#include "../model_variant/e547_strips_pal.c"
