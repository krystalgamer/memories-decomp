#define D_8009B145_IN_DATA
#define GRAPHICS_ACTIVE_BUFFER_IN_DATA
#include "../types.h"
#include "campaign_flags.h"
#include "display_object_config.h"
#include "scene_script.h"
#include "script_command_busy.h"
#include "fade.h"
#include "fade_overlay_controls.h"
#include "display_object.h"
#include "display_object_core.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#define GRAPHICS_VIEWPORT_IN_DATA
#include "graphics_frame.h"
#include "image_transfer_rects.h"
#include "script_state.h"
#include "script_image_objects.h"
#include "script_image_rebuild.h"
#include "script_image_commands.h"
#include "script_flag_commands.h"
#include "../unmatched.h"

#define VRAM_COPY_WIDTH 0x140
#define VRAM_COPY_HEIGHT 0xA0

/* Regional values, as in script_image_rebuild.c: the image sits at
   (0, 0xF0) in the US build and at (0x280, 0xD0) in the European one,
   whose wrapper defines its own. */
#ifndef SCRIPT_IMAGE_SCREEN_X
#define SCRIPT_IMAGE_SCREEN_X 0
#endif
#ifndef SCRIPT_IMAGE_SCREEN_Y
#define SCRIPT_IMAGE_SCREEN_Y 0xF0
#endif
#define SCRIPT_IMAGE_OBJECT_SET_VIEW(set) ((ScriptImageObjectSet *)(set))

void Script_OpStageImage(void){register unsigned char*p=gScript_StreamCursor;register unsigned char*p2=p+2;register unsigned char*p4;unsigned short value;gScript_ViewportTargetY=0;gScript_ViewportTargetX=0;gScript_StreamCursor=p2;value=p[0]|(p[1]<<8);gScript_ImageOperand=value;if(value&SCRIPT_IMAGE_OPERAND_HAS_VIEWPORT){p4=p+4;gScript_StreamCursor=p4;gScript_ViewportTargetX=p[2]|(p2[1]<<8);gScript_StreamCursor=p+6;gScript_ViewportTargetY=p[4]|(p4[1]<<8);}gScript_wCommandWord=5;}

void Script_OpShowImage(void) {
    DisplayObject *rec;
    u16 flags;
    s32 ret;
    s32 masked;
    u32 mask;

    ret = ScriptCommand_MarkStarted();
    mask = DISPLAY_OBJECT_ATTRIBUTE_16BPP;
    if (ret == 0) {
        gGraphics_aImageTransferRects[0].x = 0;
        gGraphics_aImageTransferRects[0].y = 0;
        gGraphics_aImageTransferRects[0].w = VRAM_COPY_WIDTH;
        gGraphics_aImageTransferRects[0].h = VRAM_COPY_HEIGHT;
        if (gGraphics_bActiveBuffer == 0) {
            gGraphics_aImageTransferRects[0].x = VRAM_COPY_WIDTH;
        }
        MoveImage(&gGraphics_aImageTransferRects[0], 0x1C0, 0x100);
        return;
    }

    mask |= 0x30;
    masked = gFile_dwTransferState_abs & mask;
    if ((masked | gFile_dwCancelRequestState_abs) != 0) {
        return;
    }
    if (gFade_State.flags & FADE_FLAG_ACTIVE) {
        return;
    }

    flags = gScript_wCommandWord;
    if (!(flags & SCRIPT_SHOW_IMAGE_CONTROL_TRANSFER_PHASE)) {
        gScript_wCommandWord = flags | SCRIPT_SHOW_IMAGE_CONTROL_TRANSFER_PHASE;
        rec = DisplayObject_AcquireSlot(DisplayObject_FindFreeSlot(), 3);
        DisplayObject_ConfigureScreenSprite(rec, 0, 0, VRAM_COPY_WIDTH,
            VRAM_COPY_HEIGHT, 0, 0, 0x17, SCRIPT_IMAGE_SCREEN_X,
            SCRIPT_IMAGE_SCREEN_Y + 4);
        gScript_TransitionObject = rec;
        rec->attribute |= DISPLAY_OBJECT_ATTRIBUTE_16BPP;
        ScriptImage_ReleaseObjects(SCRIPT_IMAGE_OBJECT_SET_VIEW(gSceneScript_Slots));
        gGraphics_sViewportX = gScript_ViewportTargetX;
        gGraphics_sViewportY = gScript_ViewportTargetY;
        if (gFade_bWhiteLatch == 0) {
            Fade_StartOutKeepOverlay(gFade_bWhiteLatch);
        }
        ScriptImage_RequestTransfer(
            SCRIPT_IMAGE_OBJECT_SET_VIEW(gSceneScript_Slots), gScript_ImageOperand & SCRIPT_IMAGE_OPERAND_ID_MASK);
        return;
    }
    if (!(flags & SCRIPT_SHOW_IMAGE_CONTROL_REBUILD_PHASE)) {
        gScript_wCommandWord = flags | SCRIPT_SHOW_IMAGE_CONTROL_REBUILD_PHASE;
        DisplayObject_ReleaseIfPresent(gScript_TransitionObject);
        ScriptImage_RebuildObjects(
            SCRIPT_IMAGE_OBJECT_SET_VIEW(gSceneScript_Slots), -1);
        if (gFade_bWhiteLatch == 0 && !(gScript_ImageOperand & SCRIPT_IMAGE_OPERAND_SKIP_FADE_IN)) {
            Fade_StartInKeepOverlay();
        }
        return;
    }
    gScript_wCommandWord = 0;
}

void Script_OpStoryFlag(void)
{
    u8 *cursor = gScript_StreamCursor;
    u8 *next = cursor + 2;
    u16 command;

    gScript_StreamCursor = next;
    command = cursor[0] | (cursor[1] << 8);
    if (command & CAMPAIGN_FLAG_COMMAND_WRITE) {
        Library_UpdateCardUsedFlag(command & CAMPAIGN_FLAG_COMMAND_PAYLOAD_MASK);
    } else {
        u32 offset;

        gScript_StreamCursor = cursor + 4;
        offset = cursor[2] | (next[1] << 8);
        if (Campaign_TestStoryFlag(command))
            gScript_StreamCursor = D_801A8000 + offset;
    }
    gScript_wCommandWord = 0;
}

void Script_OpViewportTween(void)
{
    {
        u8 *cursor = gScript_StreamCursor;

        gScript_StreamCursor = cursor + 2;
        gScript_ViewportTargetX = cursor[0] | (cursor[1] << 8);
    }
    {
        u8 *cursor = gScript_StreamCursor;

        gScript_StreamCursor = cursor + 2;
        gScript_ViewportTargetY = cursor[0] | (cursor[1] << 8);
    }
    {
        u8 *cursor = gScript_StreamCursor;

        gScript_StreamCursor = cursor + 2;
        gScript_SharedOperand = cursor[0] | (cursor[1] << 8);
    }
    gScript_wCommandWord = 7;
}

void Script_UpdateViewportTween(void) {
    s32 sx;
    s32 sy;
    s32 n;
    s32 v;
    s32 a;
    s32 b;
    s32 c;
    s32 d;

    if (ScriptCommand_MarkStarted() == 0) {
        sx = gGraphics_sViewportX;
        n = *(s16 *)&gScript_SharedOperand;
        gScript_ViewportDeltaX = ((*(s16 *)&gScript_ViewportTargetX - sx) << 16) / n;
        sy = gGraphics_sViewportY;
        gScript_ViewportDeltaY = ((*(s16 *)&gScript_ViewportTargetY - sy) << 16) / n;
        gScript_ViewportAccumulatorX = (sx << 16) | 0x8000;
        gScript_ViewportAccumulatorY = (sy << 16) | 0x8000;
    }

    a = gScript_ViewportAccumulatorX;
    do { b = gScript_ViewportDeltaX; } while (0);
    c = gScript_ViewportAccumulatorY;
    d = gScript_ViewportDeltaY;
    a = a + b;
    gScript_ViewportAccumulatorX = a;
    gGraphics_sViewportX = a >> 16;
    c = c + d;
    gScript_ViewportAccumulatorY = c;
    gGraphics_sViewportY = c >> 16;

    v = gScript_SharedOperand - 1;
    gScript_SharedOperand = v;
    if ((s16)v <= 0) {
        gScript_wCommandWord = 0;
        gGraphics_sViewportX = gScript_ViewportTargetX;
        gGraphics_sViewportY = gScript_ViewportTargetY;
    }
}
