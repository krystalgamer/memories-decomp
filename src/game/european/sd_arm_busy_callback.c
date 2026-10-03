#include "../../types.h"

/* SLES-03947 build of src/game/sd_arm_busy_callback.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define g_SDValue (*(SDValue *G32 *)0x8009C3C0)
#define D_8009B128 (*(void (**)(void))0x8009C050)

#include "../sd_arm_busy_callback.c"
