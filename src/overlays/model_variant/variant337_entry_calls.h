#ifndef FRENCH_MODEL_VARIANT337_ENTRY_CALLS_H
#define FRENCH_MODEL_VARIANT337_ENTRY_CALLS_H

#include "../../types.h"

/* The header-337 entry's data block and helper calls. They live apart from
 * variant337_entry.h, whose layout types other families include. */
extern GsIMAGE gFrenchModelVariant337_aImages[9];
extern Variant337EntryConfig gFrenchModelVariant337_aConfigs[];
void ModelVariant320_DrawRibbon(Variant337EntryState *context);
void SpanishModelVariant337_DrawRings(Variant337EntryState *context);
void ModelVariant320_DrawStreamers(Variant337EntryState *context);

#endif
