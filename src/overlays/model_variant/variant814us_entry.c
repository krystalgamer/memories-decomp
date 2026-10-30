#include "../../types.h"

/* The US header-814 entry is the French header-831 entry with the flare
   palette at the NTSC CLUT row 0x3D20; the linker symbols supply the
   resident call targets. */
#define VARIANT831_FLARE_CLUT 0x3D20
#include "../french_model_variant/variant831_entry.c"
