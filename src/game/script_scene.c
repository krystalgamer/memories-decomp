#include "../types.h"

#include "script_scene.h"
#include "script_state.h"

/* Zero-initialized storage this unit owns. Each build's generated
 * script_scene_commons subsegment places it at the retail addresses. */
u16 gScript_ImageOperand;
struct MenuRecord *gScript_pMenuRecord;
s16 gScript_WaitCounter;
u8 gCampaignSceneIndex;
u16 gScript_wCommandWord;
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

#include "fade.h"
#include "file_transfer.h"
#include "graphics_frame.h"
#include "script_state.h"
#include "script_image_objects.h"
#include "script_image_rebuild.h"

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

#include "duel_effect.h"
#include "text_channel_state.h"
#include "duel_effect_set_entry_range_layout.h"
#include "text_box_lifecycle.h"
#include "script_state.h"
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

#include "secondary_scene_commands.h"
#include "script_state.h"
#include "main_mode_state.h"

void Script_OpOpenCampaignMap(void)
{
    unsigned char value = *gScript_StreamCursor++;

    gMain_bActiveModeState = 5;
    gCampaignMap_bEntryLocation = value;
}

#include "display_effect_lifecycle.h"
#include "display_effect_process_menu_records.h"
#include "menu_record.h"
#include "display_menu_records.h"
#include "menu_record_reset.h"
#include "script_state.h"

void Script_OpShowMenu(void)
{
    MenuRecord *o;
    s32 first;
    s32 flags;
    s32 bit;

    if (ScriptCommand_MarkStarted() == 0) {
        first = *gScript_StreamCursor++;
        flags = *gScript_StreamCursor++;
        bit = flags & 0x80;

        o = &gDisplayEffect_aMenuRecords[bit >> 7];
        gScript_pMenuRecord = o;

        if (o->effect_id >= 0) {
            DisplayEffect_ReleaseRecord(o);
        }
        DisplayEffect_ResetMenuRecords();
        DisplayEffect_ResetRecordState((DisplayEffectState *)o);

        o->resource_entry_index = flags & 15;
        o->effect_id = first;
        o->slot_index = bit >> 7;
        o->display_effect_step = 2;
        if (bit) {
            o->x = 0xD8;
        }
    }

    DisplayEffect_ProcessMenuRecords(0);
    if (gScript_pMenuRecord->display_effect_step == 0) {
        gScript_wCommandWord = 0;
    }
}

#include "sound.h"
#include "file_transfer.h"
#include "script_state.h"
#include "sound_bgm_commands.h"

void Script_OpSound(void)
{
    u8 *q;
    s32 c;
    s32 f;

    if (ScriptCommand_MarkStarted() == 0) {
        q = gScript_StreamCursor;
        c = *q;
        q++;
        gScript_StreamCursor = q;
        f = c;

        if ((f & 0x3F) == 0) {
            if (f & 0x40) {
                gScript_StreamCursor = q + 2;
                SD_SEPlayFull(q[0] | (q[1] << 8));
            } else {
                gScript_StreamCursor = q + 2;
                SD_BGMPlay(q[0] | (q[1] << 8));
            }
        } else {
            if (f & 1) {
                SD_BGMPlay(gSD_dwSavedBgmCommand);
            }
            if (f & 2) {
                q = gScript_StreamCursor;
                gScript_StreamCursor = q + 2;
                gSD_dwSavedBgmCommand = q[0] | (q[1] << 8);
            }
            if (f & 4) {
                gSD_dwSavedBgmCommand = gSD_dwCurrentBgmCommand;
            }
        }
        if (f & 0x80) {
            return;
        }
    } else if ((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
               gFile_dwCancelRequestState) {
        return;
    }
    gScript_wCommandWord = 0;
}

void Script_OpFadeBgm(void)
{
    if (ScriptCommand_MarkStarted() == 0) {
        u8 *stream = gScript_StreamCursor;
        u32 raw = *stream;
        u32 argument;
        s32 command;

        gScript_StreamCursor = stream + 1;
        argument = raw & 0x7F;
        command = raw;
        if (argument == 0) {
            SD_BGMFadeOut();
        } else {
            SD_BGMFadeOutWithStep(argument);
        }
        if (command & 0x80) {
            return;
        }
    }
    gScript_wCommandWord = 0;
}

void Script_OpJump(void)
{
    u8 *source = gScript_StreamCursor;
    u32 offset;

    gScript_StreamCursor = source + 2;
    offset = source[0] | (source[1] << 8);
    gScript_wCommandWord = 0;
    gScript_StreamCursor = gFile_StagingBuffer + offset;
}

#include "card_constants.h"
#include "data_transfer_request.h"
#include "duel_effect_mode_7.h"
#include "input.h"
#include "save_data.h"

#include "duel_effect.h"
#include "text_channel_state.h"
#include "text_box_lifecycle.h"
#include "sound.h"
#include "text_box_runtime.h"
#include "text_box_update_channels.h"
#include "duel_effect_set_entry_range_layout.h"
#include "display_object.h"
#include "display_object_interpolation.h"
#include "script_state.h"
#include "main_services.h"
#include "dialog_choice_control.h"
#include "dialog_read_choice_input.h"
#include "duel_effect_mark_object_if_active.h"
#include "duel_effect_entry_control.h"
#include "dialog_choice.h"
#include "main_mode_state.h"
#include "save_data_workspace.h"

#define DISPLAY_OBJECT_X_HALFWORD_INDEX \
    ((u32)&((DisplayObject *)0)->xy.h.x / sizeof(s16))
#define DISPLAY_OBJECT_Y_HALFWORD_INDEX \
    ((u32)&((DisplayObject *)0)->xy.h.y / sizeof(s16))

int Duel_IsPlayerDeckComplete(void)
{
    unsigned short *entry = gDuel_awPlayerDeck;
    int i = 0;

    while (i < DECK_SIZE) {
        if (*entry == 0) {
            return 0;
        }
        i++;
        entry++;
    }
    return 1;
}

#if defined(VERSION_JAPAN)
/* The Japanese build makes no DuelEffect_SetEntryRangeLayout(0, 2) call before any of the
 * prompt's boxes, sizes three of them differently, and confirms with Circle
 * or Square. */
#define SAVE_PROMPT_PREPARE_BOX()
#define SAVE_PROMPT_INITIAL_BOX_HEIGHT 0x30
#define SAVE_PROMPT_SLIDE_BOX_SIZE 0x70, 0x40
#define SAVE_PROMPT_SLIDE_BOX_FLAGS 0x24
#define SAVE_PROMPT_CHOICE_BOX_RECT 0x50, 0x30, 0x20
#define SAVE_PROMPT_CHOICE_BOX_FLAGS 0x20
#define SAVE_PROMPT_DECK_ERROR_BOX_HEIGHT 0x30
#define SAVE_PROMPT_DECK_ERROR_BOX_FLAGS 0x1008
#define SAVE_PROMPT_DECK_ERROR_COMPLETION_FLAG 8
#define SAVE_PROMPT_CONFIRM_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#elif defined(VERSION_EUROPE)
#define SAVE_PROMPT_PREPARE_BOX()
#define SAVE_PROMPT_INITIAL_BOX_HEIGHT 0x40
#define SAVE_PROMPT_SLIDE_BOX_SIZE 0x78, 0x40
#define SAVE_PROMPT_SLIDE_BOX_FLAGS 0x48
#define SAVE_PROMPT_CHOICE_BOX_RECT 0x70, 0x18, 0x20
#define SAVE_PROMPT_CHOICE_BOX_FLAGS 0x40
#define SAVE_PROMPT_DECK_ERROR_BOX_HEIGHT 0x30
#define SAVE_PROMPT_DECK_ERROR_BOX_FLAGS 0x1010
#define SAVE_PROMPT_DECK_ERROR_COMPLETION_FLAG 0x10
#define SAVE_PROMPT_CONFIRM_MASK PAD_BUTTON_CONFIRM_MASK
#else
#define SAVE_PROMPT_PREPARE_BOX() DuelEffect_SetEntryRangeLayout(0, 2)
#define SAVE_PROMPT_INITIAL_BOX_HEIGHT 0x30
#define SAVE_PROMPT_SLIDE_BOX_SIZE 0x78, 0x30
#define SAVE_PROMPT_SLIDE_BOX_FLAGS 0x24
#define SAVE_PROMPT_CHOICE_BOX_RECT 0x70, 0x18, 0x18
#define SAVE_PROMPT_CHOICE_BOX_FLAGS 0x20
#define SAVE_PROMPT_DECK_ERROR_BOX_HEIGHT 0x24
#define SAVE_PROMPT_DECK_ERROR_BOX_FLAGS 0x1008
#define SAVE_PROMPT_DECK_ERROR_COMPLETION_FLAG 8
#define SAVE_PROMPT_CONFIRM_MASK PAD_BUTTON_CONFIRM_MASK
#endif
#define SAVE_PROMPT_CHANNEL(index) (&gText_aChannels[index])

/* Save-prompt dialog step for the campaign script.
 *
 * On entry it reads the prompt's text id and scene index from the script
 * stream and opens the prompt box. Afterwards it drives the memory-card
 * dialog, the confirm and cancel boxes and the slide-in/out of the prompt,
 * and finally applies the choice read from the pad.
 *
 * Two allocation levers are load-bearing. The choice handling at the end
 * sits in a one-pass do/while: GCC weights the box pointer's references by
 * that loop depth, so the box takes $s0 and its display object $s1, as
 * retail has them. `value` carries both the text id and the prompt's Y
 * coordinate, so it spans blocks and the stream's low byte keeps its own
 * register instead of being tied to the id. */
void Script_OpSavePrompt(void)
{
    DuelEffectChannel *box;
    DisplayObject *obj;
    u8 *p;
    u8 *p2;
    DisplayObject *slot;
    DuelEffectChannel *chan;
    DuelEffectChannel *prompt;
    s32 value;
    s32 lo;
    s32 choice;
    s32 step;
    u16 flags;
    u16 next;

    if (ScriptCommand_MarkStarted() == 0) {
        p = gScript_StreamCursor;
        p2 = p + 2;
        gDialog_bChoice = -1;
        gScript_StreamCursor = p2;
        lo = p[0];
        value = lo | (p[1] << 8);
        gScript_StreamCursor = p + 4;
        gScript_SavePromptSceneOperand = p[2] | (p2[1] << 8);
        SAVE_PROMPT_PREPARE_BOX();
        DuelEffect_MarkObjectIfActive(
            TextBox_Create(
                0, value, 0x10, 0xB0, 0x120, SAVE_PROMPT_INITIAL_BOX_HEIGHT
            ));
        return;
    }

    if ((gScript_wCommandWord & SCRIPT_SAVE_PROMPT_CONTROL_CARD_DIALOG) != 0) {
        if (MemCardDialog_Poll() == 0) {
            return;
        }
        gDialog_bChoiceCount = 4;
        gDialog_bChoice = 0;
        gDialog_bChoiceControl = 0;
        gScript_wCommandWord &= 0xFF7F;
        return;
    }

    TextBox_UpdateChannels();
    flags = gScript_wCommandWord;
    if ((flags & SCRIPT_SAVE_PROMPT_CONTROL_FOLLOWUP_PHASE) == 0) {
        chan = gText_aChannels;
        if ((chan->flags_34 & 0x2000) == 0) {
            return;
        }
        if (DuelEffect_HasActiveEntry(chan) != 0) {
            return;
        }
        gScript_wCommandWord |= SCRIPT_SAVE_PROMPT_CONTROL_FOLLOWUP_PHASE;
        if (Duel_IsPlayerDeckComplete() == 0) {
            gScript_wCommandWord |= SCRIPT_SAVE_PROMPT_CONTROL_DECK_ERROR;
            SD_SEPlayFull(0x2A);
            return;
        }
        SAVE_PROMPT_PREPARE_BOX();
        box = TextBox_Create(3, 0x11, -0x90, 0x38, SAVE_PROMPT_SLIDE_BOX_SIZE);
        DuelEffect_MarkObjectIfActive(box);
        box->flags_34 |= SAVE_PROMPT_SLIDE_BOX_FLAGS;
        do {
            TextBox_UpdateChannels();
        } while (box->prompt_object == 0);
        DisplayObject_SavePosition((DisplayObjectSnapshot *)box->render_object);
        slot = box->render_object;
        next = gScript_wCommandWord | 0x6000;
        slot->field_60 = -0x400;
        gScript_wCommandWord = next;
        return;
    }

    box = SAVE_PROMPT_CHANNEL(3);
    obj = box->render_object;

    if ((flags & SCRIPT_SAVE_PROMPT_CONTROL_EXIT_CONFIRM) != 0) {
        if ((flags & SCRIPT_SAVE_PROMPT_CONTROL_SUBPHASE_STARTED) == 0) {
            gScript_wCommandWord = flags | SCRIPT_SAVE_PROMPT_CONTROL_SUBPHASE_STARTED;
            SAVE_PROMPT_PREPARE_BOX();
            box = TextBox_Create(2, 0x12, 0x90, SAVE_PROMPT_CHOICE_BOX_RECT);
            DuelEffect_MarkObjectIfActive(box);
            box->flags_34 |= SAVE_PROMPT_CHOICE_BOX_FLAGS;
            do {
                TextBox_UpdateChannels();
            } while (box->prompt_object == 0);
        }
        prompt = SAVE_PROMPT_CHANNEL(2);
        if ((prompt->flags_34 & 0x2000) == 0) {
            return;
        }
        gScript_wCommandWord &= 0xF3FF;
        TextBox_Destroy(prompt);
        if (gDialog_bChoice != 0) {
            D_8009B268 = 1;
            gMain_bRequestedMenuID = 5;
            gMain_bActiveModeState = 8;
        }
        gDialog_bChoiceCount = 4;
        gDialog_bChoice = 2;
        return;
    }

    if ((flags & SCRIPT_SAVE_PROMPT_CONTROL_DECK_ERROR) != 0) {
        if ((flags & SCRIPT_SAVE_PROMPT_CONTROL_SUBPHASE_STARTED) == 0) {
            gScript_wCommandWord = flags | SCRIPT_SAVE_PROMPT_CONTROL_SUBPHASE_STARTED;
            SAVE_PROMPT_PREPARE_BOX();
            DuelEffect_MarkObjectIfActive(TextBox_CreateFlagged(
                0,
                0x1C,
                0x10,
                0xB0,
                0x120,
                SAVE_PROMPT_DECK_ERROR_BOX_HEIGHT,
                SAVE_PROMPT_DECK_ERROR_BOX_FLAGS));
            return;
        }
        if ((SAVE_PROMPT_CHANNEL(0)->flags_34 &
             SAVE_PROMPT_DECK_ERROR_COMPLETION_FLAG) != 0) {
            return;
        }
        SD_SEPlayFull(8);
        D_8009B268 = 1;
        gMain_bRequestedMenuID = 5;
        gMain_bActiveModeState = 8;
        return;
    }

    if ((flags & SCRIPT_SAVE_PROMPT_CONTROL_SLIDE_OUT) != 0) {
        if ((flags & SCRIPT_SAVE_PROMPT_CONTROL_SUBPHASE_STARTED) == 0) {
            gScript_wCommandWord = flags | SCRIPT_SAVE_PROMPT_CONTROL_SUBPHASE_STARTED;
            DisplayObject_SavePosition((DisplayObjectSnapshot *)obj);
            *(s16 *)&box->render_object->field_60 = 0x400;
        }
        step = *(u16 *)&obj->field_60 - 0x40;
        obj->field_60 = step;
        if ((s16)step <= 0) {
            TextBox_Destroy(box);
            gScript_wCommandWord = 0;
            return;
        }
        Widget_SlideSine((DisplayObjectPosition *)obj, -0x90, 0x38, (s16)step);
        TextBox_SetPos(box, *(s16 *)&obj->xy.h.x,
                       *(s16 *)&obj->xy.h.y);
        return;
    }

    if ((flags & SCRIPT_SAVE_PROMPT_CONTROL_SLIDE_IN) != 0) {
        step = *(u16 *)&obj->field_60 + 0x40;
        obj->field_60 = step;
        if ((s16)step >= 0) {
            *(s16 *)&obj->xy.h.x = 0x10;
            *(s16 *)&obj->xy.h.y = 0x38;
            gScript_wCommandWord = flags & 0xDFFF;
            /* These reads keep the halfword-array spelling: direct member
               access costs sixteen bytes, while derived halfword indexes keep
               the accepted allocation tied to the shared layout. */
            value = ((s16 *)obj)[DISPLAY_OBJECT_Y_HALFWORD_INDEX];
            TextBox_SetPos(
                box, ((s16 *)obj)[DISPLAY_OBJECT_X_HALFWORD_INDEX], value
            );
            return;
        }
        Widget_SlideSine((DisplayObjectPosition *)obj, 0x10, 0x38, (s16)step);
        TextBox_SetPos(box, *(s16 *)&obj->xy.h.x,
                       *(s16 *)&obj->xy.h.y);
        return;
    }

    do {
        if (Dialog_ReadChoiceInput(box) != 0) {
            return;
        }
        if ((gInput_awPadPressed[0] & SAVE_PROMPT_CONFIRM_MASK) == 0) {
            return;
        }
        ((SaveDataWorkspace *)gSaveData_Workspace)->state.campaign_scene.word =
            gScript_SavePromptSceneOperand;
        choice = gDialog_bChoice;
        switch (choice) {
        case 0:
            SD_SEPlayFull(7);
            ((SaveDataWorkspace *)gSaveData_Workspace)->state.campaign_scene.word =
                gScript_SavePromptSceneOperand;
            SaveData_RequestWrite();
            gScript_wCommandWord |= SCRIPT_SAVE_PROMPT_CONTROL_CARD_DIALOG;
            break;
        case 1:
            SD_SEPlayFull(7);
            BuildDeck_EnterNarrowConfirmMode();
            gMain_bReturnMode = 2;
            gCampaignSceneIndex = (u8)gScript_SavePromptSceneOperand;
            break;
        case 2:
            SD_SEPlayFull(7);
            gScript_wCommandWord |= SCRIPT_SAVE_PROMPT_CONTROL_EXIT_CONFIRM;
            break;
        case 3:
            SD_SEPlayFull(8);
            gScript_wCommandWord |= SCRIPT_SAVE_PROMPT_CONTROL_SLIDE_OUT;
            break;
        }
    } while (0);
}

void Script_OpUnusedHalt14(void)
{
}

void Script_OpUnusedHalt15(void)
{
}

#include "fade.h"
#include "script_state.h"

void Script_OpFadeOut(void)
{
    int command;

    if (ScriptCommand_MarkStarted() == 0) {
        command = *gScript_StreamCursor++;
        Fade_InitOut();
        if (command & 0x3F) {
            gFade_State.step = command & 0x3F;
        }
        if ((command & 0x80) == 0) {
            goto clear;
        }
        goto end;
    }
    if (gFade_State.flags & 0x80) {
        goto end;
    }
clear:
    gScript_wCommandWord = 0;
end:;
}

#include "display_asset_banks.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"

void DuelResult_LoadPackageStage(FileTransferDescriptor *d, s32 mode)
{
    s32 one;
    s32 c;
    s32 m;
    s32 m2;
    s32 n;
    s32 t0;
    s32 u0;
    s32 t1;
    s32 t2;
    s32 v0;
    s32 v1;

    one = 1;
    switch (mode) {
    case 0:
        do {
            m = 0xFFDDFFFF;
        } while (0);
        do {
            n = 0x18000;
        } while (0);
        /* Stored through the members' addresses with the other signedness
           (and s32 for the u32 phase_size), so fold leaves them as non-struct
           stores. Spelling any of the four as a member store changes 8 to 22
           words of this case. */
        *(s16 *)&d->field_30.h.counter = 0x1C0;
        *(s16 *)&d->field_30.h.field_32 = 0x100;
        t0 = gFile_dwTransferState;
        *(u16 *)&d->w = 0x40;
        gFile_dwTransferState = t0 & m;
        u0 = gFile_dwTransferState;
        *(s32 *)&d->phase_size = n;
        gFile_dwTransferState = u0 | 0x10000;
        d->transfer_mode = 2;
        v0 = gFile_TransferBufferBase;
        do {
            d->h = 0x10;
        } while (0);
        d->sector_destination = v0;
        d->alternate_sector_destination = v0 + FILE_SECTOR_SIZE;
        break;

    case 1:
        m2 = 0xFFDCFFFF;
        d->phase_size = FILE_SECTOR_SIZE;
        t1 = gFile_dwTransferState;
        v1 = gFile_TransferBufferBase;
        gFile_dwTransferState = t1 & m2;
        d->alternate_sector_destination = v1;
        d->sector_destination = v1;
        goto join;

    case 2:
#ifdef VERSION_EUROPE
        /* The European image goes to (0x280, 0xD4). */
        d->x = 0x280;
        d->y = 0xD4;
        d->h = mode;
        c = gFile_TransferBufferBase;
        d->w = 0x100;
#else
        d->y = 0xF4;
        d->h = mode;
        c = gFile_TransferBufferBase;
        d->x = 0;
        d->w = 0x100;
#endif
        LoadImage2((RECT *)d, (u32 *)c);
        m2 = 0xFFDCFFFF;
        d->alternate_sector_destination = (s32)gDisplay_abResourceBank0;
        d->sector_destination = (s32)gDisplay_abResourceBank0;
        t2 = gFile_dwTransferState;
        d->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState = t2 & m2;
join:
        d->transfer_mode = one;
        break;
    }
}

#include "display_object_config.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "display_object_core.h"
#include "display_object_layout.h"
#include "fade.h"
#include "display_object_helpers.h"
#include "file_transfer.h"
#include "script_state.h"
#include "script_image_objects.h"
#include "sound_output.h"
#include "display_asset_banks.h"

#ifdef VERSION_EUROPE
#include "duel_effect_resource_setup.h"
#endif

/* The menu assets' first sector in WA.MRG. A regional build defines its own. */
#ifndef SCRIPT_DUEL_RESULT_MENU_ASSETS_START_SECTOR
#define SCRIPT_DUEL_RESULT_MENU_ASSETS_START_SECTOR FILE_WA_MENU_ASSETS_START_SECTOR
#endif

/* Duel result screen setup. Reads the two-byte result code from the script
   stream, queues the result sector through File_RequestAsyncTransfer with
   DuelResult_LoadPackageStage as the completion callback, and on the first pass (once the
   transfer and fade flags are clear) builds the screen: the banner object
   from gDisplay_abResourceBank0, the two 0x140-wide panels, and the white-on-black fill
   object kept in gScript_TransitionObject, then starts the music. On later passes it
   waits for the fade, releases the fill object and the panel list, and
   clears the phase flag. */
void Script_OpDuelResult(void) {
    u8 *p;
    DisplayObject *o;
    DisplayObject *q;
    u16 flags;
    s32 four;
    s32 color;

    if (ScriptCommand_MarkStarted() == 0) {
        p = gScript_StreamCursor;
        gScript_StreamCursor = p + 2;
        gScript_SharedOperand = p[0] | (p[1] << 8);
        File_RequestAsyncTransfer(
            0, 0,
            SCRIPT_DUEL_RESULT_MENU_ASSETS_START_SECTOR,
            FILE_WA_MENU_ASSETS_SECTOR_COUNT,
            DuelResult_LoadPackageStage, 0, 0
        );
        File_WaitForTransfers();
    }
    flags = gScript_wCommandWord;
    if ((flags & 0x4000) == 0) {
        if (((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             gFile_dwCancelRequestState) != 0) {
            return;
        }
        gScript_wCommandWord = flags | 0x4000;
        if (gCampaign_PrimaryDisplayObject != 0) {
            gCampaign_PrimaryDisplayObject->flags &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
        }
        ScriptImage_ReleaseObjects((ScriptImageObjectSet *)gSceneScript_Slots);
#ifdef VERSION_EUROPE
        /* The European banner takes gText_bLanguageIndex as its sixth argument, one
           0x140 by 0x100 panel stands in for the US two, and the fill quad
           is initialised in mode 1 with corners at y 0, 0x40 and 0x100. */
        four = 4;
        o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(o, 0, 0, 0, 0, gText_bLanguageIndex, 0x17, four, gDisplay_abResourceBank0);
        DisplayObject_SetDepthOffset(o, 1);
        o->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                    DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        o->attribute |= DISPLAY_OBJECT_ATTRIBUTE_8BPP;
        gSceneScript_Slots[0].object_address = (s32)o;
        o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1);
        DisplayObject_ConfigureScreenSprite(o, 0, 0, 0x140, 0x100, 0, 0, 0x19, 0x280, 0xD5);
        DisplayObject_SetDepthOffset(o, -1);
        gSceneScript_Slots[1].object_address = (s32)o;
        gSceneScript_Slots[2].object_address = 0;
        q = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), four);
        DisplayObject_InitializeGouraudQuad(q, 1);
        q->xy.word = 0x140;
        q->field_38.word = 0x400000;
        q->position.word = 0;
        q->field_40.word = 0x400140;
        q->field_48.word = 0x1000000;
        q->field_50.word = 0x1000140;
        q->field_2C.word = 0xFFFFFF;
        q->field_34.word = 0xFFFFFF;
        q->field_4C = 0xFFFFFF;
        q->field_54 = (void *)0xFFFFFF;
        gScript_TransitionObject = q;
        q->attribute |= (GsALON | GsATWO);
#else
        four = 4;
        o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(o, 0, 0, 0, 0, 0, 0x17, four, gDisplay_abResourceBank0);
        DisplayObject_SetDepthOffset(o, 1);
        o->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                    DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        o->attribute |= DISPLAY_OBJECT_ATTRIBUTE_8BPP;
        gSceneScript_Slots[0].object_address = (s32)o;
        o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1);
        DisplayObject_ConfigureScreenSprite(o, 0, 0, 0x140, 0xF0, 0, 0, 0x19, 0, 0xF5);
        DisplayObject_SetDepthOffset(o, -1);
        gSceneScript_Slots[1].object_address = (s32)o;
        o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1);
        DisplayObject_ConfigureScreenSprite(o, 0x100, 0, 0x40, 0xF0, 0, 0, 0x19, 0, 0xF5);
        DisplayObject_SetDepthOffset(o, -1);
        gSceneScript_Slots[2].object_address = (s32)o;
        q = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), four);
        DisplayObject_InitializeGouraudQuad(q, 0);
        color = 0xF00140;
        q->xy.word = 0x140;
        q->field_38.word = 0xF00000;
        q->field_3C.word = 0xFFFFFF;
        q->field_44.word = 0xFFFFFF;
        q->position.word = 0;
        q->field_40.word = color;
        gScript_TransitionObject = q;
        q->attribute |= (GsALON | GsATWO);
#endif
        Fade_StartIn();
        gFade_State.step = four;
        Fade_Wait();
    } else {
        if (SD_GetStatusFlags() & 0x80) {
            return;
        }
        Fade_WaitOut();
        if (gCampaign_PrimaryDisplayObject != 0) {
            gCampaign_PrimaryDisplayObject->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
        }
        DisplayObject_ReleaseIfPresent(gScript_TransitionObject);
        ScriptImage_ReleaseObjects((ScriptImageObjectSet *)gSceneScript_Slots);
        gScript_wCommandWord = 0;
    }
}

#include "script_state.h"
#include "main_mode_state.h"

void Script_OpGameOver(void)
{
    gMain_bActiveModeState = 12;
    gMain_bReturnMode = 12;
}

void Script_OpCredits(void)
{
    gMain_bActiveModeState = 15;
    gMain_bReturnMode = 15;
}

void Script_OpWait(void)
{
    if (ScriptCommand_MarkStarted() == 0) {
        u8 *p = gScript_StreamCursor;
        gScript_StreamCursor = p + 2;
        gScript_WaitCounter = p[0] | (p[1] << 8);
    }
    gScript_WaitCounter--;
    if (gScript_WaitCounter <= 0)
        gScript_wCommandWord = 0;
}

#include "../psyq/rand.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "text_box_update_channels.h"
#include "script_command_table.h"
#include "script_state.h"
#include "main_services.h"
#include "main_mode_state.h"

/* Initialized data this unit owns at 0x80090C50: the 23 scene-script command handlers.
 *
 * Script_RunTick masks the command word with 0x1F before indexing, while
 * TextBox_BuildStep can redispatch the current low byte directly. The image
 * contains exactly these entries, so the declaration remains unsized rather
 * than implying every masked value is valid.
 */

ScriptCommandHandler gScript_apfnCommandHandlers[] = {
    Script_OpHalt,
    Script_OpLoadImageScene,
    Script_OpShowDialog,
    Script_OpStoryFlag,
    Script_OpStageImage,
    Script_OpShowImage,
    Script_OpViewportTween,
    Script_UpdateViewportTween,
    Script_OpOpenCampaignMap,
    Script_OpShowMenu,
    Script_OpSound,
    Script_OpFadeBgm,
    Script_OpJump,
    Script_OpSavePrompt,
    Script_OpUnusedHalt14,
    Script_OpUnusedHalt15,
    Script_OpFadeOut,
    Script_OpDuelResult,
    Script_OpGameOver,
    Script_OpCredits,
    Script_OpWait,
    Script_OpJumpIfDeckIncomplete,
    Script_OpReturnToMenu,
};

/* Regional value: the completion mask it tests (0x2008 -> 0x2010). The
 * European build (src/game/european/) defines its own. */
#ifndef SCRIPT_RUN_TICK_COMPLETION_MASK
#define SCRIPT_RUN_TICK_COMPLETION_MASK TEXT_BOX_COMPLETION_MASK
#endif

void Script_OpJumpIfDeckIncomplete(void) {
    unsigned char *current = gScript_StreamCursor;
    unsigned int offset;
    gScript_StreamCursor = current + 2;
    offset = current[0] | (current[1] << 8);
    if (Duel_IsPlayerDeckComplete() == 0) gScript_StreamCursor = gFile_StagingBuffer + offset;
    gScript_wCommandWord = 0;
}

void Script_OpReturnToMenu(void)
{
    D_8009B268 = 1;
    gMain_bRequestedMenuID = 5;
    gMain_bActiveModeState = 8;
}

void Script_RunTick(void)
{
    u16 f;
    s32 g;
    s32 b;

    rand();
    f = gScript_EventState;
    if (f & DUEL_EVENT_SCRIPT_FLAG_DIALOG_ACTIVE) {
        TextBox_UpdateChannels();
        if ((*(u32 *)&gText_aChannels[0].flags_34 & SCRIPT_RUN_TICK_COMPLETION_MASK) == TEXT_BOX_FLAG_DONE) {
            gScript_EventState &= ~DUEL_EVENT_SCRIPT_FLAG_DIALOG_ACTIVE;
        }
        return;
    }
    if (f == 0) {
        return;
    }
    if ((f & DUEL_EVENT_SCRIPT_FLAG_STARTED) == 0) {
        g = gScript_EventState;
        gScript_StreamCursor = gFile_StagingBuffer + ((u16 *)gFile_StagingBuffer)[g];
        b = *gScript_StreamCursor++;
        gScript_DialogCommandLatch = 0;
        gScript_EventState = g | DUEL_EVENT_SCRIPT_FLAG_STARTED;
        gScript_wCommandWord = b;
    }
    if (gScript_DialogCommandLatch != 0) {
        gScript_wCommandWord = gScript_DialogCommandLatch;
    }
    while (1) {
        gScript_apfnCommandHandlers[gScript_wCommandWord & 0x1F]();
        if (gScript_wCommandWord != 0) {
            return;
        }
        gScript_wCommandWord = *gScript_StreamCursor++;
    }
}

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "duel_load_package_stage.h"
#include "../ygo_types.h"

#include "high_memory_addresses.h"
#include "shared_work_base.h"

/* Regional values, which the European wrapper defines for itself: the
   sectors stage 0 transfers (16 in the US build, 12 in the European one)
   and where stage 2 uploads its image row, (0x100, 0xF0) in the US build
   and (0x280, 0xE0) in the European one. */
#ifndef CAMPAIGN_SCENE_STAGE0_SECTORS
#define CAMPAIGN_SCENE_STAGE0_SECTORS 16
#endif
#ifndef CAMPAIGN_SCENE_IMAGE_X
#define CAMPAIGN_SCENE_IMAGE_X 0x100
#endif
#ifndef CAMPAIGN_SCENE_IMAGE_Y
#define CAMPAIGN_SCENE_IMAGE_Y 0xF0
#endif

void Campaign_LoadScenePackageStage(FileTransferDescriptor *p, s32 stage)
{
    s32 v_0;
    s32 v_1;
    s32 v_2;
    s32 v_3;
    s32 w;
    s32 t;
    s32 m;
    s32 c;

    switch (stage) {
    case 0:
        *(s16 *)&p->field_30.h.counter = 0x340;
        *(u16 *)&p->w = 0x40;
        m = 0xFFDDFFFF;
        v_0 = gFile_dwTransferState;
        *(u16 *)&p->h = 0x10;
        v_0 &= m;
        gFile_dwTransferState = v_0;
        w = gFile_dwTransferState;
        *(s16 *)&p->field_30.h.field_32 = 0;
        w |= 0x10000;
        gFile_dwTransferState = w;
        p->transfer_mode = 2;
        t = (p->sector_destination = gFile_TransferBufferBase);
        *(s32 *)&p->phase_size = CAMPAIGN_SCENE_STAGE0_SECTORS * FILE_SECTOR_SIZE;
        p->alternate_sector_destination = t + FILE_SECTOR_SIZE;
        break;

    case 1:
        *(s32 *)&p->phase_size = FILE_SECTOR_SIZE;
        m = 0xFFDCFFFF;
        v_1 = gFile_dwTransferState;
        t = gFile_TransferBufferBase;
        v_1 &= m;
        gFile_dwTransferState = v_1;
        p->alternate_sector_destination = t;
        p->sector_destination = t;
        p->transfer_mode = 1;
        break;

    case 2:
        c = gFile_TransferBufferBase;
        p->x = CAMPAIGN_SCENE_IMAGE_X;
        p->y = CAMPAIGN_SCENE_IMAGE_Y;
        p->w = 0x100;
        p->h = 1;
        LoadImage2((RECT *)p, (u32 *)c);
        *(s32 *)&p->alternate_sector_destination = (s32)gFile_StagingBuffer;
        *(s32 *)&p->sector_destination = (s32)gFile_StagingBuffer;
        m = 0xFFDCFFFF;
        v_2 = gFile_dwTransferState;
        p->phase_size = 0x1000;
        v_2 &= m;
        gFile_dwTransferState = v_2;
        p->transfer_mode = 1;
        break;

    case 3:
        *(s32 *)&p->phase_size = CAMPAIGN_DIALOG_PORTRAIT_PHASE_SIZE;
        m = 0xFFDCFFFF;
        v_3 = gFile_dwTransferState;
        t = (s32)gRuntime_pSharedWorkBase;
        v_3 &= m;
        gFile_dwTransferState = v_3;
        p->alternate_sector_destination = t;
        p->sector_destination = t;
        p->transfer_mode = 1;
        break;
    }
}

/* The campaign scene package and its scene-script slots, in address order:
   Campaign_LoadScenePackage requests the scene package, clears the three
   gSceneScript_Slots records and uploads the dialog portraits; the
   scene-script record callbacks and SceneScript_UpdateSlots then drive the
   live records through gSceneScript_RecordCallbacks, which this unit owns. */

#include "display_object.h"
#include "file_transfer.h"
#include "graphics_frame.h"
#include "image_transfer_rects.h"
#include "menu_record_reset.h"
#include "script_state.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

#include "high_memory_addresses.h"
#include "shared_work_base.h"
#include "trig_constants.h"
#include "scene_script_record_callbacks.h"

/* The package's first sector in WA.MRG. A regional build defines its own. */
#ifndef CAMPAIGN_SCENE_PACKAGE_START_SECTOR
#define CAMPAIGN_SCENE_PACKAGE_START_SECTOR FILE_WA_CAMPAIGN_SCENE_START_SECTOR
#endif

#ifndef CAMPAIGN_SCENE_PACKAGE_SECTOR_COUNT
#define CAMPAIGN_SCENE_PACKAGE_SECTOR_COUNT FILE_WA_CAMPAIGN_SCENE_SECTOR_COUNT
#endif

#define SCENE_SCRIPT_SLOT_SIGNED_HEAD(slot) (*(s16 *)(slot))
#define CAMPAIGN_PACKAGE_WORDS(data) ((u32 *)(data))

/* Loads the fixed scene presentation/event package. The argument initializes
 * gScript_EventState for the event driver; it does not select a different disc page.
 * The 0x1E57/0x31 request uses Campaign_LoadScenePackageStage to route image,
 * palette, event script, and portrait data. After resetting viewport and slot
 * state, this function polls IsIdleGPU(10) until it returns zero, then uploads
 * 25 pairs from the arena held in gRuntime_pSharedWorkBase: 24-word by 48-row image
 * rectangles on a five-wide grid from x=0x380, followed by 64x1 CLUT
 * rectangles from y=0xF0.
 *
 * The clearing loop subscripts the record array rather than walking a
 * pointer. Both spellings give 112 instructions, but the walking pointer adds
 * a reference to `slot` and that changes which pseudo the allocator ranks
 * first: the %hi temporary for gSceneScript_Slots then takes $v0 and the address is
 * completed as `addiu $s0,$v0`, where retail coalesces the pair into
 * `lui $s0` / `addiu $s0,$s0` and emits it after the callback address rather
 * than before. Subscripting drops that reference and the prologue falls into
 * retail's order. */
void Campaign_LoadScenePackage(s16 scene_index)
{
    SceneScriptSlot *slot;
    u32 *src;
    RECT *second;
    s32 i;

    slot = gSceneScript_Slots;
    gScript_EventState = scene_index;
    gGraphics_sViewportY = 0;
    gGraphics_sViewportX = 0;
    gCampaign_PrimaryDisplayObject = 0;
    File_RequestAsyncTransfer(
        0, 0,
        CAMPAIGN_SCENE_PACKAGE_START_SECTOR,
        CAMPAIGN_SCENE_PACKAGE_SECTOR_COUNT,
        Campaign_LoadScenePackageStage, 0, 0
    );
    DisplayEffect_ResetMenuRecords();

    SCENE_SCRIPT_SLOT_SIGNED_HEAD(&slot[3]) = -1;
    for (i = 0; i < 3; i++) {
        slot[i].object_address = 0;
        slot[i].callback_control = 0;
    }
    File_WaitForTransfers();

    while (IsIdleGPU(10)) {
        ;
    }

    src = CAMPAIGN_PACKAGE_WORDS(gRuntime_pSharedWorkBase);
    gGraphics_aImageTransferRects[0].x = 0x380;
    gGraphics_aImageTransferRects[0].y = 0;
    gGraphics_aImageTransferRects[0].w = CAMPAIGN_DIALOG_PORTRAIT_IMAGE_WORD_WIDTH;
    gGraphics_aImageTransferRects[0].h = CAMPAIGN_DIALOG_PORTRAIT_IMAGE_HEIGHT;
    second = &gGraphics_aImageTransferRects[1];
    gGraphics_aImageTransferRects[1].x = 0x380;
    second->y = 0xF0;
    second->w = CAMPAIGN_DIALOG_PORTRAIT_CLUT_WORD_WIDTH;
    second->h = 1;

    i = 0;
    do {
        LoadImage2(&gGraphics_aImageTransferRects[0], src);
        LoadImage2(
            &gGraphics_aImageTransferRects[1],
            src + CAMPAIGN_DIALOG_PORTRAIT_IMAGE_SIZE / sizeof(u32)
        );
        i++;
        gGraphics_aImageTransferRects[1].y = gGraphics_aImageTransferRects[1].y + 1;
        gGraphics_aImageTransferRects[0].x =
            (i % CAMPAIGN_DIALOG_PORTRAIT_GRID_COLUMN_COUNT) *
                CAMPAIGN_DIALOG_PORTRAIT_IMAGE_WORD_WIDTH + 0x380;
        gGraphics_aImageTransferRects[0].y =
            (i / CAMPAIGN_DIALOG_PORTRAIT_GRID_COLUMN_COUNT) *
                CAMPAIGN_DIALOG_PORTRAIT_IMAGE_HEIGHT;
        if (gGraphics_aImageTransferRects[1].y >=
            0xF0 + CAMPAIGN_DIALOG_PORTRAIT_CLUT_ROWS_PER_COLUMN) {
            gGraphics_aImageTransferRects[1].x =
                gGraphics_aImageTransferRects[1].x +
                CAMPAIGN_DIALOG_PORTRAIT_CLUT_WORD_WIDTH;
            gGraphics_aImageTransferRects[1].y = 0xF0;
        }
        src += CAMPAIGN_DIALOG_PORTRAIT_RECORD_SIZE / sizeof(u32);
    } while (i < CAMPAIGN_DIALOG_PORTRAIT_COUNT);

    gCampaign_PrimaryDisplayObject = Campaign_CreatePrimaryDisplayObject();
}

/* Initialized data this unit owns at 0x80090CAC: the two callbacks SceneScript_UpdateSlots selects
 * from each live scene-script record's byte at +4. Their native prototypes
 * differ, so the table keeps the generic two-argument call shape used by its
 * sole consumer and casts at the initializer boundary.
 */

SceneScriptRecordCallback gSceneScript_RecordCallbacks[] = {
    (SceneScriptRecordCallback)SceneScript_NoOpRecordCallback,
    (SceneScriptRecordCallback)SceneScript_UpdateColorPulse,
};

/* The two scene-script record callbacks, in the order gSceneScript_RecordCallbacks
   (this unit) holds them: SceneScript_UpdateSlots picks
   one by each live record's byte at +4. */

void SceneScript_NoOpRecordCallback(void)
{
}

#define DISPLAY_OBJECT_COLOR_BYTES(object) ((u8 *)&(object)->rgb)

void SceneScript_UpdateColorPulse(SceneScriptSlot *state, DisplayObject *color)
{
    s32 angle;
    s32 intensity;
    s32 component;
    DisplayObject *object;

    if (((u16)state->callback_control & 0x8000) == 0) {
        state->callback_control |= 0x8000;
        state->pulse_angle = 0;
    }
    angle = ((u16)state->pulse_angle + 32) & TRIG_ANGLE_MASK;
    state->pulse_angle = angle;
    intensity = rsin(angle) * 24 / ONE;
    if (intensity == 0)
        intensity = 1;
    if (intensity == 24)
        intensity = 23;
    component = intensity - 104;
    DISPLAY_OBJECT_COLOR_BYTES(color)[2] = component;
    DISPLAY_OBJECT_COLOR_BYTES(color)[1] = component;
    DISPLAY_OBJECT_COLOR_BYTES(color)[0] = component;
    object = (DisplayObject *)gSceneScript_Slots[0].object_address;
    if (object != 0) {
        component = (intensity + 24) / 2 - 128;
        DISPLAY_OBJECT_COLOR_BYTES(object)[2] = component;
        DISPLAY_OBJECT_COLOR_BYTES(object)[1] = component;
        DISPLAY_OBJECT_COLOR_BYTES(object)[0] = component;
    }
    object = (DisplayObject *)gSceneScript_Slots[1].object_address;
    if (object != 0) {
        component = (intensity + 24) / 2 - 128;
        DISPLAY_OBJECT_COLOR_BYTES(object)[2] = component;
        DISPLAY_OBJECT_COLOR_BYTES(object)[1] = component;
        DISPLAY_OBJECT_COLOR_BYTES(object)[0] = component;
    }
}

void SceneScript_UpdateSlots(SceneScriptSlot *records) {
    s32 i;
    for (i = 0; i < 3; i++) {
        s32 value = records->object_address;
        if (value != 0) {
            gSceneScript_RecordCallbacks[*(u8 *)&records->callback_control](records, value);
        }
        records++;
    }
}

#include "script_state.h"

int Script_ReadByte(void)
{
    return *gScript_StreamCursor++;
}

int Script_ReadU16LE(void)
{
    u8 *current = gScript_StreamCursor;

    gScript_StreamCursor = current + 2;
    return current[0] | (current[1] << 8);
}
