#ifndef GAME_MODEL_CAMERA_STATE_H
#define GAME_MODEL_CAMERA_STATE_H

#include "../types.h"
#include "model.h"

#ifndef MODEL_CAMERA_MOVE_CUSTOM_EXTERN
extern ModelCameraMove gModel_CameraMove;
#endif
extern s16 gModel_wCameraFollowCountdown;
extern s16 gModel_wCameraTargetSlot[];

#ifdef MODEL_HANDLER_VIEW_OFFSET_ABSOLUTE
extern u32 gModel_AnimatedBattleCameraEndpoint[];
#else
extern u32 gModel_AnimatedBattleCameraEndpoint[2];
#endif

#ifdef MODEL_HANDLER_OFFSET_ABSOLUTE
extern u32 gModel_ExodiaOrbitOffset[];
#else
extern u32 gModel_ExodiaOrbitOffset[2];
#endif

#endif
