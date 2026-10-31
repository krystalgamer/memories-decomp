#include "../../types.h"

/* The USA build of the French MODEL60348 entry prints its work size after
   clearing it and was compiled with GCC 2.8.0. The format string follows the
   entry, so the tail data sits 0x28 later than in the French images. */
#define MODEL_IMAGE60348_WORK_SIZE_PRINTF
#define MODEL_IMAGE60348_WORK_SIZE_FORMAT D_8013D6D8
#define D_8013D6C8 D_8013D6F0
#define D_8013D71C D_8013D744

static const char D_8013D6D8[24] = "E_DATA size = %x byte\n";

#include "../french_model_variant/image60348_entry.c"
