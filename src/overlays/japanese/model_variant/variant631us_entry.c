#include "../../../types.h"
#define MODEL631US_STRING_PAD "\xA2"

/* The US header-631 build of the French header-648 entry adds the E_DATA
   work-size printf, which shifts the image header and command rows 0x28 later. */
#define MODEL_IMAGE9840_WORK_SIZE_PRINTF
#define MODEL_IMAGE9840_WORK_SIZE_FORMAT D_8013CD64

#ifndef MODEL631US_STRING_PAD
#endif
static const char D_8013CD64[24] = "E_DATA size = %x byte\n\0" MODEL631US_STRING_PAD;

#include "../../french_model_variant/image9840_entry.c"
