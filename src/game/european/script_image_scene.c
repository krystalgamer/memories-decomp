#include "../../types.h"

/* SLES-03947 build of src/game/script_image_scene.c, with its European bodies. */

#define VERSION_EUROPE

/* Values that differ in the European release; the US source names each
 * with an #ifndef default. */
#define SCRIPT_DIALOG_BOX_HEIGHT 0x40
#define SCRIPT_DIALOG_BOX_FLAG 0x10
#define SCRIPT_IMAGE_SCREEN_X 0x280
#define SCRIPT_IMAGE_SCREEN_Y 0xD0

#define SCRIPT_IMAGE_SCENE_REGIONAL_SMALL_STORAGE
/* This build's own zero-initialized storage at 0x8009C1F0-0x8009C230, defined
 * in address order (#2602); its layout differs from the North American block
 * the shared source defines. */
u16 gScript_wCommandWord;
u16 gScript_SharedOperand;
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

#include "../script_image_scene.c"
