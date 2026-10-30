#include "../../types.h"

/* The North American header-383 streamers reload the roll word into itself
 * (see variant400_streamers.c). */
#define MODEL_VARIANT400_STREAMERS_RELOAD_ROLL
#define func_8013CDB8 func_8013CD7C
#include "../french_model_variant/variant400_streamers.c"
