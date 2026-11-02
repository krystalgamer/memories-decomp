#ifndef MEMORIES_DECOMP_MODEL_SCENE_CONTROLLERS_DATA_H
#define MEMORIES_DECOMP_MODEL_SCENE_CONTROLLERS_DATA_H

#include "../types.h"

/* Zero-initialized data that model_scene_controllers.c defines (#2602), for the units that
 * reach it without the unit's other declarations. */

extern s16 D_800F2B22;

/* One consumer, Model_ApplyTextureTint. These four contiguous halfwords are filled
 * as a RECT for StoreImage2, LoadImage2, and MoveImage. The linker assignments
 * establish the scratch storage, but no subsystem header owns it. */
extern s16 gModel_sTextureTintRectX;
extern s16 gModel_sTextureTintRectY;
extern s16 gModel_sTextureTintRectW;
extern s16 gModel_sTextureTintRectH;
#ifdef D_8009B_MODEL_VISIBLE
extern ModelBytes8 gModel_ViewSnapshot;
#endif

#endif
