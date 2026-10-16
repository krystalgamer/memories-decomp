#ifndef MEMORIES_DECOMP_FUNC_8001D518_H
#define MEMORIES_DECOMP_FUNC_8001D518_H

#include "../types.h"
#include "display_object.h"

/* Returns the cached action-ready marker, or creates one linked to source.
 * The caller checks eligibility and publishes the result to D_8009B1F8.
 * A cached object is returned unchanged, without rebinding its source. */
DisplayObject *Duel_GetOrCreateActionReadyMarker(DisplayObject *source);

#endif
