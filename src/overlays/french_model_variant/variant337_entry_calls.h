#ifndef FRENCH_MODEL_VARIANT337_ENTRY_CALLS_H
#define FRENCH_MODEL_VARIANT337_ENTRY_CALLS_H

#include "../../types.h"

/* The header-337 entry's data block and helper calls. They live apart from
 * variant337_entry.h, whose layout types other families include. */
extern u8 gFrenchModelVariant337_D1F5C[];
void ModelVariant320_DrawRibbon(u8 *context);
void SpanishModelVariant337_DrawRings(u8 *context);
void ModelVariant320_DrawStreamers(u8 *context);

#endif
