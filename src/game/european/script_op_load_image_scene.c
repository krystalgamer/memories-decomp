#include "../../types.h"
#define SCRIPT_OP_LOAD_IMAGE_SCENE_REGIONAL_SMALL_STORAGE
/* This build's own zero-initialized storage at 0x8009C1F4-0x8009C230 (#2602); its
 * layout differs from the North American block the shared source defines. */
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
u32 D_8009C210_sles_03947;
u32 D_8009C214_sles_03947;
u32 D_8009C218_sles_03947;
struct DisplayObject *gScript_TransitionObject;
u32 D_8009C220_sles_03947;
s32 gScript_ViewportAccumulatorX;
s32 gScript_ViewportAccumulatorY;
u32 D_8009C22C_sles_03947;

/* SLES-03947 build of src/game/script_op_load_image_scene.c. The US build reaches these objects
 * through second .data views named *_abs; this build names the
 * objects only. The US source is included as is. */
#define D_8009B0F4_abs D_8009B0F4
#define D_8009B134_abs D_8009B134

#include "../script_op_load_image_scene.c"
