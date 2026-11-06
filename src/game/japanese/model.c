#include "../../types.h"
/* SLPM-86398 build of src/game/model.c. VERSION_JAPAN drops the debug
 * display data and traces and selects the Japanese draw and debug paths. */
#define VERSION_JAPAN
/* Drops the control step's FntPrint trace. */
#define MODEL_CONTROL_TRACE(message)
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
s16 gModel_wViewYaw;
s16 gModel_wViewPitch;
ModelBytes8 gModel_ViewSnapshot;

#include "../model.c"
