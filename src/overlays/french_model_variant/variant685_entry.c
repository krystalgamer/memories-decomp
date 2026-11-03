#include "../../types.h"
/* The French images omit the work-size printf and its string, so the data
 * after the shorter text sits 0x28 lower; the raw data tail at 0x3520
 * stays in place, so the command table absorbs the difference. The flash
 * covers the 256-line PAL screen. */
#define VERSION_FRENCH
#include "../model_variant/variant668_entry.c"
