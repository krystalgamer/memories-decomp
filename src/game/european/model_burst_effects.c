#include "../../types.h"

/* SLES-03947 build of src/game/model_burst_effects.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define MODEL_BURST_CLUT 0x3D28

#include "../model_burst_effects.c"
