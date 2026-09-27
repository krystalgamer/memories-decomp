#include "../../types.h"

/* SLES-03947 build of src/game/input_pads.c: only the functions enabled below. */

#define VERSION_EUROPE
#define VERSION_EUROPE_INPUT_RESET_PADS
#define VERSION_EUROPE_INPUT_INIT_PADS

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define INPUT_RAW_PAD_BUFFER_SIZE 0x24
#define INPUT_REPEAT_THRESHOLD 0x14
#define INPUT_REPEAT_RELOAD_VALUE 0x11

#include "../input_pads.c"
