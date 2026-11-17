#include "../../types.h"

/* The North American header-408 streamer projects its last point against the
 * fixed first one instead of walking a pointer to the previous point. */
#define MODEL_VARIANT425_STREAMERS_FIXED_TAIL
#include "../model_variant/e591_streamers_pal.c"
