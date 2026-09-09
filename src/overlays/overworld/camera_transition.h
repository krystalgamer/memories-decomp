#ifndef MEMORIES_DECOMP_OVERWORLD_CAMERA_TRANSITION_H
#define MEMORIES_DECOMP_OVERWORLD_CAMERA_TRANSITION_H

#include "../../types.h"

/* Arms a `steps`-frame camera tween towards location `index`, taking the
   target pose from that location's record. */
void CampaignMap_StartCameraTween(s32 index, s32 steps);

/* Advances the tween by one frame and returns non-zero once the move is
   finished. The map's tick loops on this while a location change is in
   flight. */
s32 CampaignMap_UpdateLocationTransition(void);

#endif
