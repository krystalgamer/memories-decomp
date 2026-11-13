#ifndef FRENCH402_RIBBONS_VIEW_H
#define FRENCH402_RIBBONS_VIEW_H
#include "../../types.h"
#include "../model_variant/model_variant.h"

/* The French 402 and 412 entries share this ribbon drawer; it reads the
 * entry state of whichever image builds it. */
#ifdef MODEL_VARIANT412_RIBBONS
#include "variant412_entry.h"
typedef Variant412EntryState Family402RibbonOwner;
#else
#include "variant402_entry.h"
typedef Variant402EntryState Family402RibbonOwner;
#endif

void FrenchModelVariant402_DrawRibbons(u8 *context);
#endif
