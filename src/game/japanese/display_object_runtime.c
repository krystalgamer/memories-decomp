#include "../../types.h"

/* SLPM-86398 build of src/game/display_object_runtime.c: the symbols below sit at other
 * addresses in the Japanese executable and are already named there, so the
 * shared source is included under the Japanese names. */
#define gDisplayObject_ListRenderers D_80090FB0

#include "../display_object_runtime.c"
