
#include "../types.h"

#ifndef SCRIPT_IMAGE_SCENE_REGIONAL_SMALL_STORAGE
/* Zero-initialized storage this unit owns at 0x8009B270-0x8009B2B0, defined in
 * address order ahead of the headers that declare it (#2602). The European
 * wrapper defines its own layout instead. */
u16 gScript_ImageOperand;
struct MenuRecord *gScript_pMenuRecord;
s16 gScript_WaitCounter;
u8 gCampaignSceneIndex;
u16 gScript_wCommandWord;
static PADDING_VARIABLE(2);
struct DisplayObject *gScript_TransitionObject;
s32 gScript_ViewportAccumulatorX;
s32 gScript_ViewportAccumulatorY;
u16 gScript_DialogCommandLatch;
u8 *gScript_StreamCursor;
s32 gScript_ViewportDeltaX;
s32 gScript_ViewportDeltaY;
u16 gScript_SharedOperand;
struct DisplayObject *gCampaign_PrimaryDisplayObject;
u16 gScript_EventState;
u16 gScript_SavePromptSceneOperand;
u16 gScript_ViewportTargetX;
u16 gScript_ViewportTargetY;
static PADDING_VARIABLE(4);
#endif

#include "script_command_busy.h"
#include "script_state.h"

s32 ScriptCommand_MarkStarted(void)
{
    u16 value = gScript_wCommandWord;

    if (!(value & 0x8000)) {
        gScript_wCommandWord = value | 0x8000;
        return 0;
    }
    return 1;
}

void Script_OpHalt(void)
{
    ScriptCommand_MarkStarted();
}

#include "campaign_create_primary_display_object.h"
#include "display_object_core.h"
#include "display_object_layout.h"
#include "display_object_helpers.h"

DisplayObject *Campaign_CreatePrimaryDisplayObject(void) {
    DisplayObject *object =
        DisplayObject_AcquireSlot((s32)DisplayObject_FindFreeSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(object,0x10,0xB0,0,0,0,0xD,0x100);
    object->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    DisplayObject_SelectOrderingTable1(object);
    return object;
}

#include "scene_script.h"
#include "script_command_busy.h"
#include "fade.h"
#include "file_transfer.h"
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
        if (((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             gFile_dwCancelRequestState) == 0) {
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

#include "script_command_busy.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "duel_effect_set_entry_range_layout.h"
#include "text_box_lifecycle.h"
#include "script_state.h"
#include "script_op_show_dialog.h"
#include "duel_effect_mark_object_if_active.h"

/* Regional values, which the European wrapper defines for itself: the
   dialog box's height (0x30 in the US build, 0x40 in the European one)
   and the flag set on it while the command runs (8 in the US build, 0x10
   in the European one). */
#ifndef SCRIPT_DIALOG_BOX_HEIGHT
#define SCRIPT_DIALOG_BOX_HEIGHT 0x30
#endif
#ifndef SCRIPT_DIALOG_BOX_FLAG
#define SCRIPT_DIALOG_BOX_FLAG 8
#endif

void Script_OpShowDialog(void)
{
    u8 *script;
    DuelEffectChannel *box;
    s32 value;
    u16 flags;
    u16 boxflags;

    if (ScriptCommand_MarkStarted() == 0) {
        script = gScript_StreamCursor;
        gScript_StreamCursor = script + 2;
        value = script[0] | (script[1] << 8);
        gScript_EventState |= 0x4000;
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
        /* The Japanese and European builds make no DuelEffect_SetEntryRangeLayout call here. */
        DuelEffect_SetEntryRangeLayout(0, 2);
#endif
        box = TextBox_Create(0, value & 0xFFF, 0x10, 0xB0, 0x120, SCRIPT_DIALOG_BOX_HEIGHT);
        DuelEffect_MarkObjectIfActive(box);
        box->flags_34 |= SCRIPT_DIALOG_BOX_FLAG;
        if ((value & 0x8000) != 0) {
            flags = gScript_wCommandWord;
            boxflags = *(volatile u16 *)&box->flags_34;
            gScript_wCommandWord = flags | 0x4000;
            box->flags_34 = boxflags & (0xFFFF ^ SCRIPT_DIALOG_BOX_FLAG);
        }
        gScript_DialogCommandLatch = gScript_wCommandWord;
    } else {
        if ((gScript_EventState & 0x4000) == 0) {
            if ((gScript_wCommandWord & 0x4000) == 0) {
                TextBox_Destroy(gText_aChannels);
            }
            gScript_DialogCommandLatch = 0;
            gScript_wCommandWord = 0;
        }
    }
}

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
    masked = gFile_dwTransferState & mask;
    if ((masked | gFile_dwCancelRequestState) != 0) {
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
            gScript_StreamCursor = gFile_StagingBuffer + offset;
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
