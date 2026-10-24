#include "../../types.h"

/* SLES-03948 to SLES-03951 build of src/game/dialog_transition.c. */

#define VERSION_EUROPE
#define DIALOG_TRANSITION_SPOTLIGHT_Y 128
#define DIALOG_TRANSITION_CHANNEL_FLAG 8
#define DIALOG_BOX_X 0x08

#define FUNC_8003DA40_CHANNEL_FLAG 8
#include "../dialog_transition.c"
