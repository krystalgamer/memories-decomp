#include "../../types.h"
/* SLPM-86398 build of src/game/model_scene_controllers.c. */
#define VERSION_JAPAN
/* The Japanese camera move record has no field_06 (model.h). */
#define MODEL_CAMERA_MOVE_COMPACT_LAYOUT
#include "../model.h"
#define MODEL_SCENE_CONTROLLERS_REGIONAL_STORAGE
/* This build's own zero-initialized storage at 0x8009B368-0x8009B388 (#2602); its
 * layout differs from the North American block the shared source defines. */
s16 gModel_ImageCopyRectX;
s16 gModel_ImageCopyRectY;
s16 gModel_ImageCopyRectW;
s16 gModel_ImageCopyRectH;
s16 gModel_sTextureTintRectX;
s16 gModel_sTextureTintRectY;
s16 gModel_sTextureTintRectW;
s16 gModel_sTextureTintRectH;
u16 gModel_wViewDistance;
u16 gModel_wViewYaw;
u16 gModel_wViewPitch;
ModelBytes8 gModel_ViewSnapshot;

#include "../model_scene_controllers.c"
