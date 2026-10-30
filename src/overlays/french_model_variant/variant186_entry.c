#include "../../types.h"
/* The French images omit the work-size printf, so the text is 0x14 bytes
 * shorter, its string is gone from the read-only data, and the data after
 * them sit 0x24 lower. */
#define VERSION_FRENCH
#define GRAPHICS_DEFAULT_HEIGHT 256
#define D_8013D0C4 D_8013D0B0
#define D_8013D0E4 D_8013D0C0
#define D_8013D138 D_8013D114
#include "../model_variant/variant169_entry.c"
