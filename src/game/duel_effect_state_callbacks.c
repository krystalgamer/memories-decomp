#define GINPUT_PAD1_HELD_IS_AGGREGATE
#define GINPUT_PAD1_PRESSED_IS_AGGREGATE
#define D_8009B0CC_IN_DATA
#define SCRIPT_STATE_TEXT_CALLBACK_VIEWS
#include "../types.h"
#include "text_stream_read_le.h"
#include "../psyq/rand.h"
#include "dialog_choice_state.h"
#include "display_object_core.h"
#include "display_object_helpers.h"
#include "duel_effect_entry_control.h"
#include "file_constants.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#define GGRAPHICS_VIEWPORT_SIZED_UNSIGNED_IN_DATA
#include "graphics_frame.h"
#include "input.h"
#include "display_effect_lifecycle.h"
#include "menu_record.h"
#include "duel_effect.h"
#include "duel_effect_state_callbacks.h"
#include "script_state.h"
#include "sound.h"
#define SD_IS_BGM_FADE_ACTIVE_IGNORES_OBJECT
#include "sound_sequence_state.h"
#include "../unmatched.h"

void Text_BeginPageClear(DuelEffectChannel *object)
{
    u8 flags = object->playback_state;

    if ((flags & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        object->playback_state = flags | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        DuelEffect_SetEntryControlBytes(object, 3, 0);
        object->playback_state = 0x82;
    }
}

DisplayObject *Dialog_OpenChoice(DuelEffectChannel *record)
{
    DisplayObject *cursor = DisplayObject_AcquireSlot(
        (s32)DisplayObject_FindFreeSlot(), 2
    );

    DisplayObject_ConfigureSpriteAtPosition(
        cursor,
        record->rect_x + record->rect_width - 0x10,
        record->rect_y + record->rect_height - 0x10,
        3,
        0,
        0,
        11,
        0x20C
    );
    cursor->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                     DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    DisplayObject_SelectOrderingTable1(cursor);
    DisplayObject_SetDepthOffset(cursor, (s8)(record->depth_offset + 1));
    return cursor;
}

#ifndef DUEL_EFFECT_CHOICE_CONFIRM_MASK
#define DUEL_EFFECT_CHOICE_CONFIRM_MASK PAD_BUTTON_CONFIRM_MASK
#endif

void Text_WaitForPageAdvance(DuelEffectChannel*o){unsigned char f=o->playback_state;if((f&DUEL_EFFECT_STATE_FLAG_INITIALIZED)==0){o->playback_state=f|DUEL_EFFECT_STATE_FLAG_INITIALIZED;gText_PageAdvanceHoldCountdown=10;o->prompt_object=Dialog_OpenChoice(o);}else{if(gInput_wPad1Held[0]&PAD_BUTTON_SQUARE){gText_PageAdvanceHoldCountdown--;if(gText_PageAdvanceHoldCountdown<0)gText_PageAdvanceHoldCountdown=0;}else gText_PageAdvanceHoldCountdown=10;if(gText_PageAdvanceHoldCountdown!=0&&!(gInput_wPad1Pressed[0]&DUEL_EFFECT_CHOICE_CONFIRM_MASK))return;SD_SEPlayFull(11);o->playback_state=2;DisplayObject_ReleaseIfPresent(o->prompt_object);o->prompt_object=0;}}

void Text_StageScriptImage(DuelEffectChannel *state)
{
    s32 result;

    gScript_ViewportTargetY[0] = 0;
    gScript_ViewportTargetX[0] = 0;
    result = TextStream_ReadU16LE(state);
    gScript_ImageOperand[0] = result;

    if (result & SCRIPT_IMAGE_OPERAND_HAS_VIEWPORT) {
        u8 *G32 *slot =
            &((TextStreamOwner *)state)->streams[state->stream_index];
        u8 *script = *slot;
        s32 value = *script;

        *slot = script + 1;
        Base2_8009B2AA[0] = value;
        Base2_8009B2A8[0] = TextStream_ReadU16LE(state);
    }

    gText_ScriptDispatchGate = 5;
    gScript_wCommandWord[0] = 5;
    state->playback_state = 10;
}

void Text_StartViewportTween(DuelEffectChannel *object)
{
    s32 signed_value;
    s32 raw_value;

    object->playback_state = 0;
    D_8009B2A8_scalar = TextStream_ReadU16LE(object);
    D_8009B2AA_scalar = TextStream_ReadU16LE(object);
    gScript_SharedOperand = TextStream_ReadU16LE(object);

    signed_value = D_8009B2AA_scalar;
    raw_value = (u16)D_8009B2AA_scalar;
    if (signed_value >= 0x1000) {
        D_8009B2AA_scalar = raw_value - 0x1000;
        object->playback_state = 10;
    }

    gText_ScriptDispatchGate = 7;
    D_8009B27C_scalar = 7;
}

void TextBox_WaitForScriptCompletion(DuelEffectChannel *object)
{
    if (gText_ScriptDispatchGate == 0) {
        object->playback_state = 0;
    }
}

void Text_ContinueDisplayEffect(DuelEffectChannel *arg0) {
    u8 v = arg0->playback_state;
    MenuRecord *p;

    if (!(v & DUEL_EFFECT_STATE_FLAG_INITIALIZED)) {
        arg0->playback_state = v | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
    }

    p = gText_DisplayEffectRecord;

    if (p->display_effect_step != 0) {
        return;
    }

    {
        u8 w = arg0->playback_state;

        if ((w & 0x40) || p->effect_id >= 0x41) {
            arg0->playback_state = 0;

            return;
        }

        arg0->playback_state = w | 0x40;
    }

    {
        MenuRecord *q = gText_DisplayEffectRecord;

        q->display_effect_step = *(u8 *)&q->field_40;
    }

    {
        MenuRecord *r = gText_DisplayEffectRecord;

        *(u16 *)&r->field_40 = 0x68;

        if (r->slot_index != 0) {
            *(u16 *)&r->field_40 = 0xD8;
        }
    }

    {
        MenuRecord *s = gText_DisplayEffectRecord;

        *(u16 *)&s->field_42 = 0xB2;
        s->field_44 = -0x10;
    }
}

/* Same playback_state/bit80 gating as Text_WaitForDisplayEffect, but additionally calls
   DisplayEffect_ReleaseRecord(gText_DisplayEffectRecord) before clearing playback_state when the display-effect
   step is zero. */

void Text_WaitForDisplayEffectAndRelease(DuelEffectChannel *object)
{
    u8 flags;
    MenuRecord *record;

    flags = object->playback_state;
    if (!(flags & DUEL_EFFECT_STATE_FLAG_INITIALIZED)) {
        object->playback_state = flags | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
    }
    record = gText_DisplayEffectRecord;
    if (record->display_effect_step == 0) {
        DisplayEffect_ReleaseRecord(record);
        object->playback_state = 0;
    }
}

void Text_WaitForDisplayEffect(DuelEffectChannel *object)
{
    u8 flags = object->playback_state;

    if ((flags & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        object->playback_state = flags | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
    }
    if (gText_DisplayEffectRecord->display_effect_step == 0) {
        object->playback_state = 0;
    }
}

void Text_RequestDisplayEffectFadeOut(DuelEffectChannel *object)
{
    u8 flags = gText_DisplayEffectRecord->field_32;

    if ((flags & 3) == 0) {
        gText_DisplayEffectRecord->field_32 = flags | 0x10;
        gText_DisplayEffectRecord->display_effect_step = 6;
        object->playback_state = 8;
    }
}

void Text_RequestDisplayEffectCrossfade(DuelEffectChannel *object)
{
    u8 flags = gText_DisplayEffectRecord->field_32;

    if ((flags & 3) == 0) {
        gText_DisplayEffectRecord->field_32 = flags | 0x10;
        gText_DisplayEffectRecord->display_effect_step = 4;
        object->playback_state = 8;
    }
}

void DuelEffect_WaitForFileTransfer(DuelEffectChannel *object)
{
    if (((gFile_dwTransferState_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
         gFile_dwCancelRequestState_abs) == 0) {
        object->playback_state = 0;
    }
}

void Text_WaitForBgmFade(DuelEffectChannel *object)
{
    if (SD_IsBgmFadeActive(object) != 1) {
        object->playback_state = 0;
    }
}

void Text_WaitForDelay(DuelEffectChannel *object)
{
    u8 flags = object->playback_state;

    if ((flags & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        object->playback_state = flags | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        gText_WaitCounter = TextStream_ReadU16LE(object);
    }
    gText_WaitCounter--;
    if (gText_WaitCounter == 0) {
        object->playback_state = 0;
    }
}

void Text_WaitWithViewportShake(DuelEffectChannel *object)
{
    u8 flags = object->playback_state;

    if ((flags & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        object->playback_state = flags | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        gText_WaitCounter = TextStream_ReadU16LE(object);
        gText_ShakeViewportOrigin[0] = gGraphics_uViewportX[0];
        gText_ShakeViewportOrigin[1] = gGraphics_uViewportY[0];
    }
    if (gGraphics_dwSyncedFrameCount & 1) {
        gGraphics_uViewportX[0] = gText_ShakeViewportOrigin[0] + ((rand() & 7) - 4);
        gGraphics_uViewportY[0] = gText_ShakeViewportOrigin[1] + ((rand() & 3) - 2);
    }
    gText_WaitCounter--;
    if (gText_WaitCounter == 0) {
        gGraphics_uViewportX[0] = gText_ShakeViewportOrigin[0];
        gGraphics_uViewportY[0] = gText_ShakeViewportOrigin[1];
        object->playback_state = 0;
    }
}

/* The CD-sound wait polls three shared phases with a byte watchdog.
   Phase transitions re-arm 0xFF and deliberately fall through. The optional
   signed phase-two budget counts qualifying polls, not restarted transfers. */
void Text_WaitForCdSoundTransfer(DuelEffectChannel *object)
{
    DuelEffectChannel *p = object;

    if ((p->playback_state & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        p->playback_state |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        p->delay_countdown = 0xFF;
        gText_CdSoundWaitPhase = 0;
        if (gText_CdSoundPollBudget != 0) {
            p->playback_state |= 0x40;
        }
    }

    p->delay_countdown--;

    if (p->delay_countdown != 0) {
        switch (gText_CdSoundWaitPhase) {
        case 0:
            if ((gFile_dwTransferState_abs & FILE_TRANSFER_FLAG_SECTOR_RANGE) == 0) {
                return;
            }
            p->delay_countdown = 0xFF;
            gText_CdSoundWaitPhase = 1;
        case 1:
            if ((gFile_wTransferStepFlags_abs & 0x4000) == 0) {
                return;
            }
            p->delay_countdown = 0xFF;
            gText_CdSoundWaitPhase = 2;
        case 2:
            if ((gFile_wTransferStepFlags_abs & 0x4000) == 0) {
                break;
            }
            if ((p->playback_state & 0x40) == 0) {
                return;
            }
            gText_CdSoundPollBudget--;
            if (gText_CdSoundPollBudget > 0) {
                return;
            }
            break;
        }
    }

    p->playback_state = 0;
    p->delay_countdown = 1;
}
