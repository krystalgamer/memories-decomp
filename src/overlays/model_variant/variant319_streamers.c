#include "../../types.h"

/* The North American header-319 streamers reload the roll word after the turn
 * advances (see variant336_streamers.c). */
#define MODEL_VARIANT336_STREAMERS_RELOAD_ROLL
#define func_8013CB6C func_8013CB5C
#include "../french_model_variant/variant336_streamers.c"
