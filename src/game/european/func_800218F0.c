#include "../../types.h"

/* SLES-03947 build of src/game/func_800218F0.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_RESULT_STARCHIP_Y 210
#define DUEL_RESULT_RANK_Y 40

/* gFade_State under its second US name, as in the Japanese wrapper: a
 * #define would give one identifier fade.h's two declarations, so an asm
 * label binds the array to the same symbol and the header completes its type. */
extern u8 D_800E9EC8_arr[] asm("gFade_State");

#include "../func_800218F0.c"
