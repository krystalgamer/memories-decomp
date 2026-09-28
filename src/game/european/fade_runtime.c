#define FADE_BAND_COUNT 32
#define FADE_TRANSITION_STATE_SIZE 0x2A
#define FADE_STATE_ARRAY_ALIAS

#include "../../types.h"

/* Europe draws 32 bands over 256 lines. The fade array and state record are
 * the same object under two names, as in the Japanese build. */
#include "../fade_runtime.c"
