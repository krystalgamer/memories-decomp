#ifndef MEMORIES_DECOMP_OVERWORLD_CAMERA_STATE_H
#define MEMORIES_DECOMP_OVERWORLD_CAMERA_STATE_H

#include "../../types.h"

/* Loads the five camera fields of location `index` out of the 66-byte
   location record and commits them. */
void CampaignMap_SetCameraFromLocation(s32 index);

/* The per-frame view callback the map installs in D_800E9DB0[3]
   (set_location.c:58): reprograms the GTE screen, offset and fog from the
   current projection, then advances the fade if one is running. */
void CampaignMap_UpdateView(void);

/* Puts the camera back to the map's default pose and clears the three
   rotation words. */
void CampaignMap_ResetCamera(void);

/* Walks the camera with the d-pad. */
void CampaignMap_MoveCameraDpad(void);

#endif
