#include "../../types.h"

/* SLES-03947 build of src/game/ai_script_state_ops.c: every function except
 * AiScript_Print, which the US source leaves out under VERSION_EUROPE. */

#define VERSION_EUROPE

#include "../ai_script_state_ops.c"
