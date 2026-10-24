#include "../../types.h"

/* SLES-03947 build of src/game/dialog_transition.c. */

#define VERSION_EUROPE

/* Values that differ in the European release; the US source names each
 * with an #ifndef default. */
#define DIALOG_TRANSITION_SPOTLIGHT_Y 128
#define DIALOG_TRANSITION_CHANNEL_FLAG 8

#define FUNC_8003DA40_CHANNEL_FLAG 8
#include "../dialog_transition.c"
