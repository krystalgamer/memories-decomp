#ifndef MEMORIES_DECOMP_MODEL_E568_HELPERS_H
#define MEMORIES_DECOMP_MODEL_E568_HELPERS_H

#include "e568_entry_pal.h"

/* The 402 bands and strip built on the 402 record. The 412 images build the
 * same bodies on FrenchVariant412EntryState, whose header includes the 402 one, so
 * these prototypes stay out of e568_entry_pal.h. */
void FrenchModelVariant402_DrawBands(FrenchVariant402EntryState *context);
void FrenchModelVariant402_DrawStrip(FrenchVariant402EntryState *context);

#endif
