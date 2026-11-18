#ifndef MEMORIES_DECOMP_MODEL_E581_HELPERS_H
#define MEMORIES_DECOMP_MODEL_E581_HELPERS_H

#include "e581_entry.h"

/* The rings built on the 398 record. The header-441 images build the same
 * body on Variant441EntryState (h424_entry_pal.h), and the
 * rings source includes e581_entry.h for both, so this prototype stays
 * out of that header. */
void ModelVariant398_DrawRings(Variant398EntryState *context);

#endif
