#ifndef FRENCH_MODEL_VARIANT400_STREAMERS_H
#define FRENCH_MODEL_VARIANT400_STREAMERS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "e566_entry_pal.h"

/* The sheets' size words as a 0x98-stride array based at sheets[0].size:
 * retail walks them from that address; indexing work->sheets[n].size
 * bases the walk at the sheet instead. */
typedef struct {
    s32 scale;
    u8 unknown_04[sizeof(ModelVariantPointSheet) - 4];
} Model400Size;

void FrenchModelVariant400_DrawStreamers(Variant400EntryState *context);
#endif
