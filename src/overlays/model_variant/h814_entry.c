#include "../../types.h"

/* The US header-814 entry is the French header-831 entry with the flare
   palette at the NTSC CLUT row 0x3D20; the linker symbols supply the
   resident call targets. */
#define VARIANT831_FLARE_CLUT 0x3D20
#include "../model_variant/h814_entry_pal.c"
