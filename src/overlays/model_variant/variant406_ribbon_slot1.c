#include "../../types.h"

/* The North American header-406 ribbon reads its projected points through
 * pointer arithmetic and packed-word halves (see variant423_ribbon.c). */
#define MODEL_VARIANT423_RIBBON_PACKED_SCREEN
#define func_8013CCB4 func_8017CC70
#include "../french_model_variant/variant423_ribbon.c"
