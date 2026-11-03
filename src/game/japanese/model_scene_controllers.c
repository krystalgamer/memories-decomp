#include "../../types.h"
/* SLPM-86398 build of src/game/model_scene_controllers.c. */
#define VERSION_JAPAN
/* The Japanese camera move record has no field_06 (model.h). */
#define MODEL_CAMERA_MOVE_COMPACT_LAYOUT
#include "../model.h"
#define MODEL_SCENE_CONTROLLERS_REGIONAL_STORAGE
/* This build's own zero-initialized storage at 0x8009B368-0x8009B388 (#2602); its
 * layout differs from the North American block the shared source defines. */
u16 D_8009B368_slpm_86398;
u16 D_8009B36A_slpm_86398;
u16 D_8009B36C_slpm_86398;
u16 D_8009B36E_slpm_86398;
u16 D_8009B370_slpm_86398;
u16 D_8009B372_slpm_86398;
u16 D_8009B374_slpm_86398;
u16 D_8009B376_slpm_86398;
u16 D_8009B378_slpm_86398;
u16 D_8009B37A_slpm_86398;
u32 D_8009B37C_slpm_86398;
ModelBytes8 gModel_ViewSnapshot;

#include "../model_scene_controllers.c"
