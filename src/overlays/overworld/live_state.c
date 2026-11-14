#include "../../types.h"
#include "live_state.h"

/* These zeros are present in the loaded image, not zero-fill-only storage. */
DisplayObject *gCampaignMap_pLocationMarker = 0;
s32 gCampaignMap_dwTweenRefX = 0;
s32 gCampaignMap_dwTweenRefZ = 0;
s32 gCampaignMap_nTweenFramesLeft = 0;
DisplayObject *gCampaignMap_pSpotlight = 0;
s32 gCampaignMap_dwTweenRefXStep = 0;
s32 gCampaignMap_dwTweenRefZStep = 0;
s32 gCampaignMap_dwTweenYaw = 0;
s32 gCampaignMap_dwTweenPitch = 0;
u8 gCampaignMap_bTransitionFlags = 0;
s32 gCampaignMap_dwTweenYawStep = 0;
s32 gCampaignMap_dwTweenPitchStep = 0;
DisplayObject *gCampaignMap_apExitObjects[CAMPAIGN_MAP_LOCATION_OBJECT_COUNT] = {0};
s32 gCampaignMap_MoveState = 0;
u8 gCampaignMap_Location = 0;
u8 gCampaignMap_bEnterSceneState = 0;
s32 gCampaignMap_dwTweenDistance = 0;
s32 gCampaignMap_dwTweenDistanceStep = 0;
