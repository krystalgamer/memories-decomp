#include "../../types.h"
/* The French images omit the work-size printf, so the data after the
 * shorter text and its rodata sit 0x28 lower; each image binds the data
 * names at its own addresses. */
#define VERSION_FRENCH
#include "../model_variant/variant660us_entry.c"
