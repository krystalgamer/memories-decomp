#include "../../types.h"

/* SLES-03947 build of src/game/func_800339D0.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define BUILD_DECK_WIDE_BOX_Y 0x84
#define BUILD_DECK_WIDE_BOX_FLAGS 0x1050
#define BUILD_DECK_NARROW_BOX_Y 0x6C
#define BUILD_DECK_NARROW_BOX_FLAGS 0x40
#define BUILD_DECK_CONFIRM_FADE_LEVEL 0xC0
#define BUILD_DECK_CONFIRM_COMPLETION_MASK 0x2010

#include "../func_800339D0.c"
