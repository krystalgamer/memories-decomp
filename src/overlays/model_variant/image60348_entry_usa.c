#include "../../types.h"

/* The USA build of the French MODEL60348 entry prints its work size after
   clearing it and was compiled with GCC 2.8.0. The format string follows the
   entry, so the tail data sits 0x28 later than in the French images. */
#define MODEL_IMAGE60348_WORK_SIZE_PRINTF

/* The byte after the terminator is zero here; Japan's images keep a stray byte. */
#ifndef MODEL60348_STRING_PAD
#if defined(VERSION_JAPAN) && defined(MODEL_VARIANT_SLOT1)
#define MODEL60348_STRING_PAD "\x92"
#elif defined(VERSION_JAPAN)
#define MODEL60348_STRING_PAD "&"
#else
#define MODEL60348_STRING_PAD ""
#endif
#endif
static const char gModelVariant664_szWorkSizeFormat[24] = "E_DATA size = %x byte\n\0" MODEL60348_STRING_PAD;

#include "../french_model_variant/image60348_entry.c"
