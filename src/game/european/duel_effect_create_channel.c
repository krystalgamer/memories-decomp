#include "../../types.h"

/* SLES-03947 build of src/game/duel_effect_create_channel.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_EFFECT_CHANNEL_CREATE_FLAGS 0x1010
#define DIALOG_CHOICE_ADDRESS 0x8009C2B0

#include "../duel_effect_create_channel.c"
