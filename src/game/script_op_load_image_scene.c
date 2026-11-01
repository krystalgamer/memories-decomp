#include "../types.h"

/* Zero-initialized storage this unit owns at 0x8009B280-0x8009B2B0, defined in
 * address order ahead of the headers that declare it (#2602). */
struct DisplayObject *G32 gScript_TransitionObject;
s32 gScript_ViewportAccumulatorX;
s32 gScript_ViewportAccumulatorY;
u16 gScript_DialogCommandLatch;
u8 *G32 gScript_StreamCursor;
s32 gScript_ViewportDeltaX;
s32 gScript_ViewportDeltaY;
u16 gScript_SharedOperand;
struct DisplayObject *G32 gCampaign_PrimaryDisplayObject;
u16 gScript_EventState;
u16 gScript_SavePromptSceneOperand;
u16 gScript_ViewportTargetX;
u16 gScript_ViewportTargetY;
static u32 D_8009B2AC;

#include "scene_script.h"
#include "script_command_busy.h"
#include "fade.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#define GRAPHICS_VIEWPORT_IN_DATA
#include "graphics_frame.h"
#include "script_state.h"
#include "script_image_objects.h"
#include "script_image_rebuild.h"
#include "script_op_load_image_scene.h"

#define SCRIPT_IMAGE_OBJECT_SET_VIEW(set) ((ScriptImageObjectSet *)(set))

void Script_OpLoadImageScene(void)
{
    u8 *script;
    u8 *next;
    u8 *next2;
    s32 value;
    u16 flags;

    if (ScriptCommand_MarkStarted() == 0) {
        gGraphics_sViewportY = 0;
        gGraphics_sViewportX = 0;
        ScriptImage_ReleaseObjects(SCRIPT_IMAGE_OBJECT_SET_VIEW(gSceneScript_Slots));
        script = gScript_StreamCursor;
        next = script + 2;
        gScript_StreamCursor = next;
        value = script[0] | (script[1] << 8);
        gScript_ImageOperand = value;
        if ((value & SCRIPT_IMAGE_OPERAND_HAS_VIEWPORT) != 0) {
            gScript_ImageOperand = value & SCRIPT_IMAGE_OPERAND_ID_MASK;
            next2 = script + 4;
            gScript_StreamCursor = next2;
            gGraphics_sViewportX = script[2] | (next[1] << 8);
            gScript_StreamCursor = script + 6;
            gGraphics_sViewportY = script[4] | (next2[1] << 8);
        }
        ScriptImage_RequestTransfer(
            SCRIPT_IMAGE_OBJECT_SET_VIEW(gSceneScript_Slots), gScript_ImageOperand);
    }
    flags = gScript_wCommandWord;
    if ((flags & SCRIPT_LOAD_IMAGE_SCENE_CONTROL_FADE_PHASE) == 0) {
        if (((gFile_dwTransferState_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             gFile_dwCancelRequestState_abs) == 0) {
            gScript_wCommandWord = flags | SCRIPT_LOAD_IMAGE_SCENE_CONTROL_FADE_PHASE;
            ScriptImage_RebuildObjects(
                SCRIPT_IMAGE_OBJECT_SET_VIEW(gSceneScript_Slots), -1);
            Fade_StartIn();
        }
    } else {
        if ((gFade_State.flags & FADE_FLAG_ACTIVE) == 0) {
            gScript_wCommandWord = 0;
        }
    }
}
