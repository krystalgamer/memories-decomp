#ifndef MEMORIES_DECOMP_DUEL_ACTION_READY_MARKER_H
#define MEMORIES_DECOMP_DUEL_ACTION_READY_MARKER_H

#include "../types.h"
#include "display_object.h"

/* Returns the cached action-ready marker, or creates one linked to source.
 * The caller checks eligibility and publishes the result to gDuel_pActionReadyMarker.
 * A cached object is returned unchanged, without rebinding its source. */
DisplayObject *Duel_GetOrCreateActionReadyMarker(DisplayObject *source);

#endif
