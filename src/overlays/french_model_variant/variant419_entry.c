#include "../../types.h"
/* The French header-419 images rebuild the US header-402 entry with a CLUT
 * at x 640; their helpers and data sit at the French addresses. */
#define VERSION_FRENCH
#define D_8013CC48 D_8013CC4C
#define func_8013C1E0 func_8013C1DC
#define func_8013C778 func_8013C770
#include "../model_variant/variant402_entry.c"
