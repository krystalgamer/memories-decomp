#ifndef MEMORIES_DECOMP_OVERWORLD_LOCATION_MARKER_H
#define MEMORIES_DECOMP_OVERWORLD_LOCATION_MARKER_H

#include "../../types.h"
#include "../../game/display_object.h"

/* Spawns the marker sprite over location `index` and returns the display
   object, or 0 if the pool is full. */
DisplayObject *CampaignMap_CreateLocationMarker(s32 index);

#endif
