#include "../../types.h"
#include "live_state.h"

/* These zeros are present in the loaded image, not zero-fill-only storage. */
#define LIVE_STATE_DATA __attribute__((section(".data")))

MapObject *D_801695C8 LIVE_STATE_DATA = 0;
s32 gCampaignMap_dwTweenRefX LIVE_STATE_DATA = 0;
s32 gCampaignMap_dwTweenRefZ LIVE_STATE_DATA = 0;
s32 D_801695D4 LIVE_STATE_DATA = 0;
MapObject *D_801695D8 LIVE_STATE_DATA = 0;
s32 gCampaignMap_dwTweenRefXStep LIVE_STATE_DATA = 0;
s32 gCampaignMap_dwTweenRefZStep LIVE_STATE_DATA = 0;
s32 gCampaignMap_dwTweenYaw LIVE_STATE_DATA = 0;
s32 gCampaignMap_dwTweenPitch LIVE_STATE_DATA = 0;
u8 D_801695EC LIVE_STATE_DATA = 0;
s32 gCampaignMap_dwTweenYawStep LIVE_STATE_DATA = 0;
s32 gCampaignMap_dwTweenPitchStep LIVE_STATE_DATA = 0;
u8 *D_801695F8[CAMPAIGN_MAP_LOCATION_OBJECT_COUNT] LIVE_STATE_DATA = {0};
s32 gCampaignMap_MoveState LIVE_STATE_DATA = 0;
u8 gCampaignMap_Location LIVE_STATE_DATA = 0;
u8 D_8016960D LIVE_STATE_DATA = 0;
s32 gCampaignMap_dwTweenDistance LIVE_STATE_DATA = 0;
s32 gCampaignMap_dwTweenDistanceStep LIVE_STATE_DATA = 0;

#undef LIVE_STATE_DATA
