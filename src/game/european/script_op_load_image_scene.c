#include "../../types.h"
#define SCRIPT_OP_LOAD_IMAGE_SCENE_REGIONAL_SMALL_STORAGE
/* This build's own zero-initialized storage at 0x8009C1F4-0x8009C230 (#2602); its
 * layout differs from the North American block the shared source defines. The
 * globals are common symbols; split.yaml's script_op_load_image_scene_commons
 * subsegment allocates them at their retail addresses. */
struct DisplayObject *gCampaign_PrimaryDisplayObject;
s32 gScript_ViewportDeltaX;
s32 gScript_ViewportDeltaY;
u16 gScript_SavePromptSceneOperand;
u16 gScript_DialogCommandLatch;
u8 *gScript_StreamCursor;
u16 gScript_EventState;
u16 gScript_ViewportTargetX;
u16 gScript_ViewportTargetY;
u16 D_8009C20E_sles_03947;
u16 gScript_ImageOperand;
struct MenuRecord *gScript_pMenuRecord;
u8 gCampaignSceneIndex;
struct DisplayObject *gScript_TransitionObject;
s16 gScript_WaitCounter;
s32 gScript_ViewportAccumulatorX;
s32 gScript_ViewportAccumulatorY;
u32 D_8009C22C_sles_03947;

/* SLES-03947 build of src/game/script_op_load_image_scene.c. The US source is included as is. */

#include "../script_op_load_image_scene.c"
