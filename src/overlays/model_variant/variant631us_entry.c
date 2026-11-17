#include "../../types.h"

/* The US header-631 build of the French header-648 entry adds the E_DATA
   work-size printf, which shifts the image header and command rows 0x28 later. */
#define MODEL_IMAGE9840_WORK_SIZE_PRINTF

#ifndef MODEL631US_STRING_PAD
#if defined(VERSION_JAPAN) && defined(MODEL_VARIANT_SLOT1)
#define MODEL631US_STRING_PAD ""
#elif defined(VERSION_JAPAN)
#define MODEL631US_STRING_PAD "\xA2"
#else
#define MODEL631US_STRING_PAD "\x92"
#endif
#endif
static const char gModelVariant631Us_szWorkSizeFormat[24] = "E_DATA size = %x byte\n\0" MODEL631US_STRING_PAD;

#include "../french_model_variant/image9840_entry.c"
