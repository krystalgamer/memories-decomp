#ifndef MEMORIES_DECOMP_OVERWORLD_LIVE_STATE_H
#define MEMORIES_DECOMP_OVERWORLD_LIVE_STATE_H

#include "../../types.h"
#include "../../ygo_types.h"

#define CAMPAIGN_MAP_LOCATION_OBJECT_COUNT 4

/* The camera tween: CampaignMap_StartCameraTween loads five 16.16
 * accumulators (each value << 16 | 0x8000) from D_800F2848 -- yaw (angle),
 * pitch (field_04), distance (field_00) and the reference point's x and z --
 * and a per-frame step towards the location record's camera; the location
 * transition adds each step and writes the integer halves back. */
extern MapObject *D_801695C8;
extern s32 gCampaignMap_dwTweenRefX;
extern s32 gCampaignMap_dwTweenRefZ;
extern s32 D_801695D4;
extern MapObject *D_801695D8;
extern s32 gCampaignMap_dwTweenRefXStep;
extern s32 gCampaignMap_dwTweenRefZStep;
extern s32 gCampaignMap_dwTweenYaw;
extern s32 gCampaignMap_dwTweenPitch;
extern u8 D_801695EC;
extern s32 gCampaignMap_dwTweenYawStep;
extern s32 gCampaignMap_dwTweenPitchStep;
extern u8 *D_801695F8[CAMPAIGN_MAP_LOCATION_OBJECT_COUNT];
extern s32 gCampaignMap_MoveState;
extern u8 gCampaignMap_Location;
extern u8 D_8016960D;
extern s32 gCampaignMap_dwTweenDistance;
extern s32 gCampaignMap_dwTweenDistanceStep;

#endif
