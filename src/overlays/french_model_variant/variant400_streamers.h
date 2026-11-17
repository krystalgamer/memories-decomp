#ifndef FRENCH_MODEL_VARIANT400_STREAMERS_H
#define FRENCH_MODEL_VARIANT400_STREAMERS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "variant400_entry_pal.h"

typedef struct {
    s32 scale;
    u8 unknown_04[0x94];
} Model400Size;

void FrenchModelVariant400_DrawStreamers(Variant400EntryState *context);
#endif
