#include "../types.h"

#include "script_scene.h"

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
s16 gScript_ViewportTargetY;

#include "display_object_system.h"
#include "display_object_layout.h"
#include "display_object.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "file_transfer.h"
#ifdef SCRIPT_IMAGE_LOCALIZED_IDS
#include "card_preview.h"
#endif

/* Regional values: the image row's upload position, (0, 0xF0) in the US build
   and (0x280, 0xD0) in the European one, whose wrapper defines its own. */
#ifndef SCRIPT_IMAGE_SCREEN_X
#define SCRIPT_IMAGE_SCREEN_X 0
#endif
#ifndef SCRIPT_IMAGE_SCREEN_Y
#define SCRIPT_IMAGE_SCREEN_Y 0xF0
#endif

void ScriptImage_TransferCallback(FileTransferDescriptor *obj, s32 mode)
{
    switch (mode) {
    case 0:
        obj->cursor.image.y = 0x100;
        obj->cursor.image.x = 0;
        obj->rect.w = 0x40;
        obj->rect.h = 0x10;
        gFile_dwTransferState &= 0xFFDDFFFF;
        gFile_dwTransferState |= 0x10000;
        obj->transfer_mode = 2;
        /* callback_data carries the request's sector count less one here
           (ScriptImage_RequestTransfer); other callers keep a pointer in it. */
        obj->phase_size = (s32)obj->callback_data << FILE_SECTOR_SHIFT;
        obj->sector_destination = gFile_TransferBufferBase;
        obj->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 1:
        obj->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        obj->alternate_sector_destination = gFile_TransferBufferBase;
        obj->sector_destination = gFile_TransferBufferBase;
        obj->transfer_mode = 1;
        break;

    case 2:
        obj->rect.x = SCRIPT_IMAGE_SCREEN_X;
        obj->rect.y = SCRIPT_IMAGE_SCREEN_Y;
        obj->rect.w = 0x100;
        obj->rect.h = 4;
        /* The descriptor's x/y/w/h prefix is the RECT LoadImage2 takes. */
        LoadImage2(&obj->rect, (u32 *)gFile_TransferBufferBase);
        break;
    }
}
#ifndef SCRIPT_IMAGE_TRANSFER_BASE
#define SCRIPT_IMAGE_TRANSFER_BASE 0x21D5
#endif

void ScriptImage_RequestTransfer(
    volatile ScriptImageObjectSet *owner, s32 value)
{
    s32 index;
    s32 mode;
    s32 stride;
    s32 base;
    FileTransferDescriptor *object;

#ifdef SCRIPT_IMAGE_LOCALIZED_IDS
    if (value == 0x10) {
        if (gText_bLanguageIndex == 1) {
            value = 0x42;
        } else if (gText_bLanguageIndex == 2) {
            value = 0x44;
        } else if (gText_bLanguageIndex == 3) {
            value = 0x46;
        } else if (gText_bLanguageIndex == 4) {
            value = 0x48;
        }
    } else if (value == 0x11) {
        if (gText_bLanguageIndex == 1) {
            value = 0x43;
        } else if (gText_bLanguageIndex == 2) {
            value = 0x45;
        } else if (gText_bLanguageIndex == 3) {
            value = 0x47;
        } else if (gText_bLanguageIndex == 4) {
            value = 0x49;
        }
    }
#endif

    if (owner) {
#ifdef SCRIPT_IMAGE_LOCALIZED_IDS
        /* The localized binaries do not repeat the shift after this store. */
        ((ScriptImageObjectSet *)owner)->image_id = value;
#else
        owner->image_id = value;
#endif
    }

    index = ((value >> 4) & 15) * 10 + (value & 15);
    mode = value >> 8;
    switch (mode) {
    case 0:
        stride = 0x21;
        base = 0;
        break;
    case 1:
        stride = 0x51;
        base = 0x672;
        break;
    case 2:
        stride = 0x71;
        base = 0x13BC;
        break;
    default:
        return;
    }
    object = File_TryRequestAsyncTransfer(
        0, 0, base + index * stride + SCRIPT_IMAGE_TRANSFER_BASE, stride,
        ScriptImage_TransferCallback, 0, 0
    );
    object->callback_data = (void *)(stride - 1);
    gFile_dwTransferState =
        object->status_flags | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
}

void ScriptImage_ReleaseObjects(ScriptImageObjectSet *set)
{
    ScriptImageEntry *entries = set->entries;
    s32 i;

    set->image_id = -1;
    for (i = 0; i < 3; i++, entries++) {
        DisplayObject_ReleaseIfPresent(entries->pointer);
        entries->pointer = 0;
        entries->callback_control = 0;
    }
}

void ScriptImage_CreateObject(ScriptImageEntry *entry, s32 size, s32 mode)
{
    ScriptImageEntry *record = entry;
    DisplayObject *object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);

    DisplayObject_ConfigureSpriteAtPosition(object, 0, 0, 2, 0, 0, size, mode);
    DisplayObject_SetDepthOffset(object, (s8)mode);
    record->object_built = 1;
    if (mode == 2) {
        record->callback_control = 1;
        object->attribute |= (GsALON | GsAONE);
    } else {
        record->callback_control = 0;
        object->attribute |= DISPLAY_OBJECT_ATTRIBUTE_8BPP;
    }
    /* Through the parameter rather than record: with every store on record,
       GCC keeps a second copy of the pointer and six instructions change. */
    entry->pointer = object;
}

/* Initialized data this unit owns at 0x80090BA8 and 0x80090C00, read only by
   ScriptImage_RebuildObjects
   when it rebuilds a scene-script image record's display objects. Both were
   part of a generated blob until now (#2602).

   gScriptImage_OverlayPairs is a ScriptImageOverlayPair per id from 0x100 up: the
   overlay-present mask, and the value written to the record at +0x2C when
   bit 1 is set.

   gScriptImage_OverlayPlacements is a ScriptImageOverlayPlacement per id from 0x200 up: the same
   mask, then an x and y for each of the two overlays, then the +0x2C value.
   Bit 7 of the mask also puts the second overlay in screen space.

   Eighty is not a multiple of six, so the last two bytes are not a row. They
   are retail padding before the script command table that follows, and the
   table record keeps them as pad_4E. */
ScriptImageOverlayPair gScriptImage_OverlayPairs[] = {
    { 3, 1 }, { 3, 1 }, { 3, 1 }, { 1, 0 },
    { 1, 0 }, { 1, 0 }, { 1, 0 }, { 1, 0 },
    { 1, 0 }, { 1, 0 }, { 1, 0 }, { 0, 0 },
    { 1, 0 }, { 0, 0 }, { 1, 0 }, { 1, 0 },
    { 1, 0 }, { 1, 0 }, { 1, 0 }, { 1, 0 },
    { 1, 0 }, { 0, 0 }, { 1, 0 }, { 0, 0 },
    { 1, 0 }, { 2, 1 }, { 1, 0 }, { 1, 0 },
    { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 3, 1 }, { 1, 0 }, { 2, 1 }, { 1, 0 },
    { 1, 0 }, { 1, 0 }, { 3, 0 }, { 3, 1 },
    { 1, 0 }, { 1, 0 }, { 0, 0 }, { 0, 0 },
};

ScriptImageOverlayPlacementTable gScriptImage_OverlayPlacements = {
    {
        { 130, 0, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 0, 0, 0 },
        { 3, 0, 0, 0, 0, 1 },
        { 3, 0, 96, 192, 0, 0 },
        { 3, 0, 0, 0, 0, 1 },
        { 2, 0, 0, 0, 0, 1 },
        { 3, 0, 0, 0, 0, 1 },
        { 3, 0, 0, 0, 0, 1 },
        { 3, 0, 0, 0, 0, 1 },
    },
    { 0, 0 },
};

/* MATCH. Real C -- this retires the ASSEMBLY-DEBT transcription (Unchiga's
 * port, a 24-instruction `__asm__` block with s0/s1 pins) that stood here.
 * gp=0, at=0, no addressing work; default flags.
 *
 * Rebuilt from the m2c draft, 36 differences to 0, five levers, every one a
 * rule already in WORKFLOW:
 *   - +0x3C and p+4 (slot 0's value) are HALFWORDS. Stored through `s32`
 *     the CENSUS read `lw +1, sw +2, sh -2, lh -1`; the difference count
 *     pointed at neither.
 *     36 -> 33, empty census.
 *   - `n` is REUSED for the decoded BCD index -- retail's `addu $s1,$v1,$v0`
 *     writes the index back into the argument's own register. 33 -> 22.
 *   - the slot's object_built store (then `p[0x10] = 1;`) goes BELOW the two
 *     read-modify-writes. 22 -> 14. The
 *     August park said "above (22 -> 19)", measured before the `n` reuse.
 *   - the gScriptImage_OverlayPairs base local is assigned AFTER the else arm's index, not
 *     before (14 -> 9), and that index is `s32 k`, not `s16 k` (9 -> 5).
 *     The August entry recorded `s16` as 8 and `s32` as 19, both measured
 *     with the three faults above open -- a spelling rejected on a broken
 *     base has not been measured.
 *   - and the last five: the flag byte REUSES `n` as well. Retail reads it
 *     `lbu $s1,0($s0)` -- into the register `n` has just vacated, which is
 *     one pseudo, not two. A separate `m` gets $s0/$s1 exchanged at all
 *     five sites. Same rule as the decode above, one value further on.
 *
 * Measured and DEAD on the finished base: `do { } while (0);` round the
 * element read (5, no change) or round the base assignment (15); the base
 * written inline with no local (+4/150); `b` assigned before the index (10).
 * And earlier, on the 9-difference base: dropping the base local (+4/145),
 * `u8 m` / `s16 m` / the flag read as `*(u8 *)(b + k*2)` (all 9), four
 * declaration orders, a named entry pointer (all 9), borrowing the other
 * arm's `t` (+2/145) or `o` (35).
 *
 * The record's three 0x14-byte image slots are ScriptImageEntry records,
 * reached as ((ScriptImageEntry *)p)[0..2], and the object each one holds
 * is a DisplayObject. ScriptImageObjectSet names the image-id halfword at
 * +0x3C immediately after those three slots.
 */

void ScriptImage_RebuildObjects(ScriptImageObjectSet *p, s32 image_id) {
    s32 n;
    DisplayObject *o;
    ScriptImageOverlayPlacement *t;
    ScriptImageOverlayPair *b;
    s32 k;

    n = image_id;
    if (n < 0) {
        n = p->image_id;
    }
    p->image_id = n;
    if (n >= 0x200) {
        o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 3);
        DisplayObject_ConfigureScreenSprite(
            o, 0, 0, 0x200, 0x100, 0, 0, 0x10,
            SCRIPT_IMAGE_SCREEN_X, SCRIPT_IMAGE_SCREEN_Y
        );
        n = ((n >> 4) & 0xF) * 10 + (n & 0xF);
        o->attribute |= DISPLAY_OBJECT_ATTRIBUTE_8BPP;
        o->flags &= ~DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        p->entries[0].object_built = 1;
        p->entries[0].pointer = o;
        t = &gScriptImage_OverlayPlacements.rows[n];
        p->entries[0].callback_control = 0;
        if (t->mask & SCRIPT_IMAGE_OVERLAY_FIRST_PRESENT) {
            ScriptImage_CreateObject(&p->entries[1],
                                     0x14, 1);
            p->entries[1].pointer->xy.h.x =
                t->x1;
            p->entries[1].pointer->xy.h.y =
                t->y1;
        }
        if (t->mask & SCRIPT_IMAGE_OVERLAY_SECOND_PRESENT) {
            ScriptImage_CreateObject(&p->entries[2],
                                     0x16, 2);
            p->entries[2].pointer->xy.h.x =
                t->x2;
            p->entries[2].pointer->xy.h.y =
                t->y2;
            p->entries[2].callback_control = t->callback_index;
            if (t->mask & SCRIPT_IMAGE_OVERLAY_SECOND_SCREEN_SPACE) {
                p->entries[2].pointer->flags |=
                    DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
            }
        }
    } else {
        ScriptImage_CreateObject(&p->entries[0], 0x10, 0);
        if (n >= 0x100) {
            k = ((n >> 4) & 0xF) * 10 + (n & 0xF);
            b = gScriptImage_OverlayPairs;
            n = b[k].mask;
            if (n & SCRIPT_IMAGE_OVERLAY_FIRST_PRESENT) {
                ScriptImage_CreateObject(&p->entries[1],
                                         0x12, 1);
            }
            if (n & SCRIPT_IMAGE_OVERLAY_SECOND_PRESENT) {
                ScriptImage_CreateObject(&p->entries[2],
                                         0x14, 2);
                p->entries[2].callback_control = b[k].callback_index;
            }
        } else {
            p->entries[1].pointer = 0;
            p->entries[2].pointer = 0;
        }
    }
}

/* Regional value: the flags bit this sets (0x2 -> 0x4). The European
 * wrapper defines its own. */
#ifndef DUEL_EFFECT_MARK_OBJECT_FLAG
#define DUEL_EFFECT_MARK_OBJECT_FLAG 2
#endif

/* Walks gDisplayEffect_aMenuRecords backward from the last record to the first; on the first
   record whose effect_id marker is non-negative, sets the flag in the
   caller's text-box channel's flags and returns. Does nothing if every
   marker is negative. */
void DuelEffect_MarkObjectIfActive(DuelEffectChannel *channel) {
    int count;
    MenuRecord *p;
    int effect_id;

    count = MENU_RECORD_COUNT - 1;
    p = &gDisplayEffect_aMenuRecords[count];
    do {
        effect_id = p->effect_id;
        count -= 1;
        if (effect_id >= 0) {
            channel->flags |= DUEL_EFFECT_MARK_OBJECT_FLAG;
            return;
        }
        p -= 1;
    } while (count >= 0);
}

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

#include "display_object_system.h"
#include "display_object_layout.h"

DisplayObject *Campaign_CreatePrimaryDisplayObject(void) {
    DisplayObject *object =
        DisplayObject_AcquireSlot(DisplayObject_FindFreeSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(object,0x10,0xB0,0,0,0,0xD,0x100);
    object->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    DisplayObject_SelectOrderingTable1(object);
    return object;
}

#include "fade.h"
#include "file_transfer.h"
#include "main.h"

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
        ScriptImage_ReleaseObjects(&gSceneScript_Slots.images);
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
            &gSceneScript_Slots.images, gScript_ImageOperand);
    }
    flags = gScript_wCommandWord;
    if ((flags & SCRIPT_LOAD_IMAGE_SCENE_CONTROL_FADE_PHASE) == 0) {
        if (((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             gFile_dwCancelRequestState) == 0) {
            gScript_wCommandWord = flags | SCRIPT_LOAD_IMAGE_SCENE_CONTROL_FADE_PHASE;
            ScriptImage_RebuildObjects(
                &gSceneScript_Slots.images, -1);
            Fade_StartIn();
        }
    } else {
        if ((gFade_State.flags & FADE_FLAG_ACTIVE) == 0) {
            gScript_wCommandWord = 0;
        }
    }
}

#include "duel_effect_states.h"
#include "duel_effect_text.h"
#include "text_helpers.h"

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
        gScript_EventState |= DUEL_EVENT_SCRIPT_FLAG_DIALOG_ACTIVE;
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
        /* The Japanese and European builds make no DuelEffect_SetEntryRangeLayout call here. */
        DuelEffect_SetEntryRangeLayout(0, 2);
#endif
        box = TextBox_Create(0, value & 0xFFF, 0x10, 0xB0, 0x120, SCRIPT_DIALOG_BOX_HEIGHT);
        DuelEffect_MarkObjectIfActive(box);
        box->flags |= SCRIPT_DIALOG_BOX_FLAG;
        if ((value & 0x8000) != 0) {
            flags = gScript_wCommandWord;
            /* Volatile keeps the reload below the command-word load; a plain
               member read is scheduled earlier and shifts the code. */
            boxflags = *(volatile u16 *)&box->flags;
            gScript_wCommandWord = flags | 0x4000;
            box->flags = boxflags & (0xFFFF ^ SCRIPT_DIALOG_BOX_FLAG);
        }
        gScript_DialogCommandLatch = gScript_wCommandWord;
    } else {
        if ((gScript_EventState & DUEL_EVENT_SCRIPT_FLAG_DIALOG_ACTIVE) == 0) {
            if ((gScript_wCommandWord & 0x4000) == 0) {
                TextBox_Destroy(gText_aChannels);
            }
            gScript_DialogCommandLatch = 0;
            gScript_wCommandWord = 0;
        }
    }
}

#include "campaign_flags.h"
#include "fade.h"
#include "display_object.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"

#define VRAM_COPY_WIDTH 0x140
#define VRAM_COPY_HEIGHT 0xA0

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
        ScriptImage_ReleaseObjects(&gSceneScript_Slots.images);
        gGraphics_sViewportX = gScript_ViewportTargetX;
        gGraphics_sViewportY = gScript_ViewportTargetY;
        if (gFade_bWhiteLatch == 0) {
            Fade_StartOutKeepOverlay(gFade_bWhiteLatch);
        }
        ScriptImage_RequestTransfer(
            &gSceneScript_Slots.images, gScript_ImageOperand & SCRIPT_IMAGE_OPERAND_ID_MASK);
        return;
    }
    if (!(flags & SCRIPT_SHOW_IMAGE_CONTROL_REBUILD_PHASE)) {
        gScript_wCommandWord = flags | SCRIPT_SHOW_IMAGE_CONTROL_REBUILD_PHASE;
        DisplayObject_ReleaseIfPresent(gScript_TransitionObject);
        ScriptImage_RebuildObjects(
            &gSceneScript_Slots.images, -1);
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
            gScript_StreamCursor = gFile_StagingBuffer.bytes + offset;
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
    s32 duration;
    s32 remaining;
    s32 accumulator_x;
    s32 delta_x;
    s32 accumulator_y;
    s32 delta_y;

    if (ScriptCommand_MarkStarted() == 0) {
        sx = gGraphics_sViewportX;
        /* The u16 globals are read signed here (retail lh); duel_effect_text.c
           reads them unsigned, so the declarations stay u16. */
        duration = *(s16 *)&gScript_SharedOperand;
        gScript_ViewportDeltaX = ((*(s16 *)&gScript_ViewportTargetX - sx) << 16) / duration;
        sy = gGraphics_sViewportY;
        gScript_ViewportDeltaY = ((gScript_ViewportTargetY - sy) << 16) / duration;
        gScript_ViewportAccumulatorX = (sx << 16) | 0x8000;
        gScript_ViewportAccumulatorY = (sy << 16) | 0x8000;
    }

    accumulator_x = gScript_ViewportAccumulatorX;
    do { delta_x = gScript_ViewportDeltaX; } while (0);
    accumulator_y = gScript_ViewportAccumulatorY;
    delta_y = gScript_ViewportDeltaY;
    accumulator_x = accumulator_x + delta_x;
    gScript_ViewportAccumulatorX = accumulator_x;
    gGraphics_sViewportX = accumulator_x >> 16;
    accumulator_y = accumulator_y + delta_y;
    gScript_ViewportAccumulatorY = accumulator_y;
    gGraphics_sViewportY = accumulator_y >> 16;

    remaining = gScript_SharedOperand - 1;
    gScript_SharedOperand = remaining;
    if ((s16)remaining <= 0) {
        gScript_wCommandWord = 0;
        gGraphics_sViewportX = gScript_ViewportTargetX;
        gGraphics_sViewportY = gScript_ViewportTargetY;
    }
}

#include "main_mode_state.h"

void Script_OpOpenCampaignMap(void)
{
    unsigned char value = *gScript_StreamCursor++;

    gMain_bActiveModeState = MAIN_MODE_CAMPAIGN_MAP;
    gCampaignMap_bEntryLocation = value;
}

#include "display_effect.h"
#include "menu_record.h"

void Script_OpShowMenu(void)
{
    MenuRecord *record;
    s32 first;
    s32 flags;
    s32 bit;

    if (ScriptCommand_MarkStarted() == 0) {
        first = *gScript_StreamCursor++;
        flags = *gScript_StreamCursor++;
        bit = flags & 0x80;

        record = &gDisplayEffect_aMenuRecords[bit >> 7];
        gScript_pMenuRecord = record;

        if (record->effect_id >= 0) {
            DisplayEffect_ReleaseRecord(record);
        }
        DisplayEffect_ResetMenuRecords();
        DisplayEffect_ResetRecordState(record);

        record->resource_entry_index = flags & 15;
        record->effect_id = first;
        record->slot_index = bit >> 7;
        record->display_effect_step = 2;
        if (bit) {
            record->x = 0xD8;
        }
    }

    DisplayEffect_ProcessMenuRecords(0);
    if (gScript_pMenuRecord->display_effect_step == 0) {
        gScript_wCommandWord = 0;
    }
}

#include "sound.h"
#include "file_transfer.h"
#include "sound_bgm_commands.h"

void Script_OpSound(void)
{
    u8 *stream;
    s32 raw;
    s32 command;

    if (ScriptCommand_MarkStarted() == 0) {
        stream = gScript_StreamCursor;
        raw = *stream;
        stream++;
        gScript_StreamCursor = stream;
        command = raw;

        if ((command & 0x3F) == 0) {
            if (command & 0x40) {
                gScript_StreamCursor = stream + 2;
                SD_SEPlayFull(stream[0] | (stream[1] << 8));
            } else {
                gScript_StreamCursor = stream + 2;
                SD_BGMPlay(stream[0] | (stream[1] << 8));
            }
        } else {
            if (command & 1) {
                SD_BGMPlay(gSD_dwSavedBgmCommand);
            }
            if (command & 2) {
                stream = gScript_StreamCursor;
                gScript_StreamCursor = stream + 2;
                gSD_dwSavedBgmCommand = stream[0] | (stream[1] << 8);
            }
            if (command & 4) {
                gSD_dwSavedBgmCommand = gSD_dwCurrentBgmCommand;
            }
        }
        if (command & 0x80) {
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
    gScript_StreamCursor = gFile_StagingBuffer.bytes + offset;
}

#include "card_constants.h"
#include "mem_card.h"
#include "build_deck.h"
#include "input.h"

#include "sound.h"
#include "display_object.h"
#include "main_services.h"
#include "main_mode_state.h"
#include "duel_effect_object_pool.h"

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
        if ((chan->flags & TEXT_BOX_FLAG_DONE) == 0) {
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
        box->flags |= SAVE_PROMPT_SLIDE_BOX_FLAGS;
        do {
            TextBox_UpdateChannels();
        } while (box->prompt_object == 0);
        DisplayObject_SavePosition(box->render_object);
        slot = box->render_object;
        next = gScript_wCommandWord | 0x6000;
        slot->field_60.phase = -0x400;
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
            box->flags |= SAVE_PROMPT_CHOICE_BOX_FLAGS;
            do {
                TextBox_UpdateChannels();
            } while (box->prompt_object == 0);
        }
        prompt = SAVE_PROMPT_CHANNEL(2);
        if ((prompt->flags & TEXT_BOX_FLAG_DONE) == 0) {
            return;
        }
        gScript_wCommandWord &= 0xF3FF;
        TextBox_Destroy(prompt);
        if (gDialog_bChoice != 0) {
            D_8009B268 = 1;
            gMain_bRequestedMenuID = 5;
            gMain_bActiveModeState = MAIN_MODE_MENU;
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
        if ((SAVE_PROMPT_CHANNEL(0)->flags &
             SAVE_PROMPT_DECK_ERROR_COMPLETION_FLAG) != 0) {
            return;
        }
        SD_SEPlayFull(SD_SE_CANCEL);
        D_8009B268 = 1;
        gMain_bRequestedMenuID = 5;
        gMain_bActiveModeState = MAIN_MODE_MENU;
        return;
    }

    if ((flags & SCRIPT_SAVE_PROMPT_CONTROL_SLIDE_OUT) != 0) {
        if ((flags & SCRIPT_SAVE_PROMPT_CONTROL_SUBPHASE_STARTED) == 0) {
            gScript_wCommandWord = flags | SCRIPT_SAVE_PROMPT_CONTROL_SUBPHASE_STARTED;
            DisplayObject_SavePosition(obj);
            box->render_object->field_60.phase = 0x400;
        }
        step = (u16)obj->field_60.phase - 0x40;
        obj->field_60.phase = step;
        if ((s16)step <= 0) {
            TextBox_Destroy(box);
            gScript_wCommandWord = 0;
            return;
        }
        Widget_SlideSine(obj, -0x90, 0x38, (s16)step);
        TextBox_SetPos(box, obj->xy.s.x,
                       obj->xy.s.y);
        return;
    }

    if ((flags & SCRIPT_SAVE_PROMPT_CONTROL_SLIDE_IN) != 0) {
        step = (u16)obj->field_60.phase + 0x40;
        obj->field_60.phase = step;
        if ((s16)step >= 0) {
            obj->xy.s.x = 0x10;
            obj->xy.s.y = 0x38;
            gScript_wCommandWord = flags & 0xDFFF;
            value = obj->xy.s.y;
            TextBox_SetPos(box, obj->xy.s.x, value);
            return;
        }
        Widget_SlideSine(obj, 0x10, 0x38, (s16)step);
        TextBox_SetPos(box, obj->xy.s.x,
                       obj->xy.s.y);
        return;
    }

    do {
        if (Dialog_ReadChoiceInput(box) != 0) {
            return;
        }
        if ((gInput_awPadPressed[0] & SAVE_PROMPT_CONFIRM_MASK) == 0) {
            return;
        }
        gSaveData_Workspace.state.campaign_scene.word =
            gScript_SavePromptSceneOperand;
        choice = gDialog_bChoice;
        switch (choice) {
        case 0:
            SD_SEPlayFull(SD_SE_DECIDE);
            gSaveData_Workspace.state.campaign_scene.word =
                gScript_SavePromptSceneOperand;
            SaveData_RequestWrite();
            gScript_wCommandWord |= SCRIPT_SAVE_PROMPT_CONTROL_CARD_DIALOG;
            break;
        case 1:
            SD_SEPlayFull(SD_SE_DECIDE);
            BuildDeck_EnterNarrowConfirmMode();
            gMain_bReturnMode = MAIN_MODE_CAMPAIGN;
            gCampaignSceneIndex = (u8)gScript_SavePromptSceneOperand;
            break;
        case 2:
            SD_SEPlayFull(SD_SE_DECIDE);
            gScript_wCommandWord |= SCRIPT_SAVE_PROMPT_CONTROL_EXIT_CONFIRM;
            break;
        case 3:
            SD_SEPlayFull(SD_SE_CANCEL);
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

void Script_OpFadeOut(void)
{
    int command;

    switch (ScriptCommand_MarkStarted()) {
    case 0:
        command = *gScript_StreamCursor++;
        Fade_InitOut();
        if (command & 0x3F) {
            gFade_State.step = command & 0x3F;
        }
        if ((command & 0x80) == 0) {
            break;
        }
        return;
    default:
        if (gFade_State.flags & FADE_FLAG_ACTIVE) {
            return;
        }
    }
    gScript_wCommandWord = 0;
}

#include "display_asset_banks.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"

void DuelResult_LoadPackageStage(FileTransferDescriptor *d, s32 mode)
{
    s32 one;
    u8 *image_buffer;
    s32 m;
    s32 m2;
    s32 n;
    s32 t0;
    s32 u0;
    s32 t1;
    s32 t2;
    u8 *v0;
    u8 *v1;

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
        *(s16 *)&d->cursor.image.x = 0x1C0;
        *(s16 *)&d->cursor.image.y = 0x100;
        t0 = gFile_dwTransferState;
        *(u16 *)&d->rect.w = 0x40;
        gFile_dwTransferState = t0 & m;
        u0 = gFile_dwTransferState;
        *(s32 *)&d->phase_size = n;
        gFile_dwTransferState = u0 | 0x10000;
        d->transfer_mode = 2;
        v0 = gFile_TransferBufferBase;
        do {
            d->rect.h = 0x10;
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
        d->transfer_mode = one;
        break;

    case 2:
#ifdef VERSION_EUROPE
        /* The European image goes to (0x280, 0xD4). */
        d->rect.x = 0x280;
        d->rect.y = 0xD4;
        d->rect.h = mode;
        image_buffer = gFile_TransferBufferBase;
        d->rect.w = 0x100;
#else
        d->rect.y = 0xF4;
        d->rect.h = mode;
        image_buffer = gFile_TransferBufferBase;
        d->rect.x = 0;
        d->rect.w = 0x100;
#endif
        /* The descriptor's x/y/w/h prefix is the RECT LoadImage2 takes. */
        LoadImage2(&d->rect, (u32 *)image_buffer);
        m2 = 0xFFDCFFFF;
        d->alternate_sector_destination = gDisplay_abResourceBank0;
        d->sector_destination = gDisplay_abResourceBank0;
        t2 = gFile_dwTransferState;
        d->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState = t2 & m2;
        d->transfer_mode = one;
        break;
    }
}

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "display_object_layout.h"
#include "fade.h"
#include "file_transfer.h"
#include "sound_driver.h"
#include "display_asset_banks.h"

#ifdef VERSION_EUROPE
#include "card_preview.h"
#endif

/* The menu assets' first sector in WA.MRG. A regional build defines its own. */
#ifndef SCRIPT_DUEL_RESULT_MENU_ASSETS_START_SECTOR
#ifdef VERSION_EUROPE
#define SCRIPT_DUEL_RESULT_MENU_ASSETS_START_SECTOR \
    (0x2528 + gText_bLanguageIndex * 50)
#else
#define SCRIPT_DUEL_RESULT_MENU_ASSETS_START_SECTOR FILE_WA_MENU_ASSETS_START_SECTOR
#endif
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
    DisplayObject *object;
    DisplayObject *fill;
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
        ScriptImage_ReleaseObjects(&gSceneScript_Slots.images);
#ifdef VERSION_EUROPE
        /* The European banner takes gText_bLanguageIndex as its sixth argument, one
           0x140 by 0x100 panel stands in for the US two, and the fill quad
           is initialised in mode 1 with corners at y 0, 0x40 and 0x100. */
        four = 4;
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0, 0, 0, gText_bLanguageIndex, 0x17, four, gDisplay_abResourceBank0);
        DisplayObject_SetDepthOffset(object, 1);
        object->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                    DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        object->attribute |= DISPLAY_OBJECT_ATTRIBUTE_8BPP;
        gSceneScript_Slots.slots[0].object = object;
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1);
        DisplayObject_ConfigureScreenSprite(object, 0, 0, 0x140, 0x100, 0, 0, 0x19, 0x280, 0xD5);
        DisplayObject_SetDepthOffset(object, -1);
        gSceneScript_Slots.slots[1].object = object;
        gSceneScript_Slots.slots[2].object = 0;
        fill = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), four);
        DisplayObject_InitializeGouraudQuad(fill, 1);
        fill->xy.word = 0x140;
        fill->field_38.word = 0x400000;
        fill->position.word = 0;
        fill->field_40.word = 0x400140;
        fill->field_48.word = 0x1000000;
        fill->field_50.word = 0x1000140;
        fill->field_2C.word = 0xFFFFFF;
        fill->field_34.word = 0xFFFFFF;
        fill->field_4C.color = 0xFFFFFF;
        fill->field_54.word = 0xFFFFFF;
        gScript_TransitionObject = fill;
        fill->attribute |= (GsALON | GsATWO);
#else
        four = 4;
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0, 0, 0, 0, 0x17, four, gDisplay_abResourceBank0);
        DisplayObject_SetDepthOffset(object, 1);
        object->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                    DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        object->attribute |= DISPLAY_OBJECT_ATTRIBUTE_8BPP;
        gSceneScript_Slots.slots[0].object = object;
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1);
        DisplayObject_ConfigureScreenSprite(object, 0, 0, 0x140, 0xF0, 0, 0, 0x19, 0, 0xF5);
        DisplayObject_SetDepthOffset(object, -1);
        gSceneScript_Slots.slots[1].object = object;
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1);
        DisplayObject_ConfigureScreenSprite(object, 0x100, 0, 0x40, 0xF0, 0, 0, 0x19, 0, 0xF5);
        DisplayObject_SetDepthOffset(object, -1);
        gSceneScript_Slots.slots[2].object = object;
        fill = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), four);
        DisplayObject_InitializeGouraudQuad(fill, 0);
        color = 0xF00140;
        fill->xy.word = 0x140;
        fill->field_38.word = 0xF00000;
        fill->field_3C.word = 0xFFFFFF;
        fill->field_44.word = 0xFFFFFF;
        fill->position.word = 0;
        fill->field_40.word = color;
        gScript_TransitionObject = fill;
        fill->attribute |= (GsALON | GsATWO);
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
        ScriptImage_ReleaseObjects(&gSceneScript_Slots.images);
        gScript_wCommandWord = 0;
    }
}

#include "main_mode_state.h"

void Script_OpGameOver(void)
{
    gMain_bActiveModeState = MAIN_MODE_GAME_OVER;
    gMain_bReturnMode = MAIN_MODE_GAME_OVER;
}

void Script_OpCredits(void)
{
    gMain_bActiveModeState = MAIN_MODE_CREDITS;
    gMain_bReturnMode = MAIN_MODE_CREDITS;
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
    if (Duel_IsPlayerDeckComplete() == 0) gScript_StreamCursor = gFile_StagingBuffer.bytes + offset;
    gScript_wCommandWord = 0;
}

void Script_OpReturnToMenu(void)
{
    D_8009B268 = 1;
    gMain_bRequestedMenuID = 5;
    gMain_bActiveModeState = MAIN_MODE_MENU;
}

void Script_RunTick(void)
{
    u16 state;
    s32 event;
    s32 command;

    rand();
    state = gScript_EventState;
    if (state & DUEL_EVENT_SCRIPT_FLAG_DIALOG_ACTIVE) {
        TextBox_UpdateChannels();
        /* Retail tests the flags with one lw over flags and
           initial_string_id; a u16 read would be lhu. */
        if ((*(u32 *)&gText_aChannels[0].flags & SCRIPT_RUN_TICK_COMPLETION_MASK) == TEXT_BOX_FLAG_DONE) {
            gScript_EventState &= ~DUEL_EVENT_SCRIPT_FLAG_DIALOG_ACTIVE;
        }
        return;
    }
    if (state == 0) {
        return;
    }
    if ((state & DUEL_EVENT_SCRIPT_FLAG_STARTED) == 0) {
        event = gScript_EventState;
        /* The staged package opens with a table of u16 script offsets. */
        gScript_StreamCursor = gFile_StagingBuffer.bytes + gFile_StagingBuffer.script_offsets[event];
        command = *gScript_StreamCursor++;
        gScript_DialogCommandLatch = 0;
        gScript_EventState = event | DUEL_EVENT_SCRIPT_FLAG_STARTED;
        gScript_wCommandWord = command;
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
#include "duel_scene.h"
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
    u8 *destination;
    s32 m;
    u8 *image_buffer;

    /* As in DuelResult_LoadPackageStage, the stores through the members'
       addresses with another type keep fold from treating them as struct
       stores. Measured one case at a time as member stores: case 0 and the
       case-2 destination pair reschedule, and cases 1 and 3 shrink the
       function by eight bytes. */
    switch (stage) {
    case 0:
        *(s16 *)&p->cursor.image.x = 0x340;
        *(u16 *)&p->rect.w = 0x40;
        m = 0xFFDDFFFF;
        v_0 = gFile_dwTransferState;
        *(u16 *)&p->rect.h = 0x10;
        v_0 &= m;
        gFile_dwTransferState = v_0;
        w = gFile_dwTransferState;
        *(s16 *)&p->cursor.image.y = 0;
        w |= 0x10000;
        gFile_dwTransferState = w;
        p->transfer_mode = 2;
        destination = (p->sector_destination = gFile_TransferBufferBase);
        p->phase_size = CAMPAIGN_SCENE_STAGE0_SECTORS * FILE_SECTOR_SIZE;
        p->alternate_sector_destination = destination + FILE_SECTOR_SIZE;
        break;

    case 1:
        *(s32 *)&p->phase_size = FILE_SECTOR_SIZE;
        m = 0xFFDCFFFF;
        v_1 = gFile_dwTransferState;
        destination = gFile_TransferBufferBase;
        v_1 &= m;
        gFile_dwTransferState = v_1;
        p->alternate_sector_destination = destination;
        p->sector_destination = destination;
        p->transfer_mode = 1;
        break;

    case 2:
        image_buffer = gFile_TransferBufferBase;
        p->rect.x = CAMPAIGN_SCENE_IMAGE_X;
        p->rect.y = CAMPAIGN_SCENE_IMAGE_Y;
        p->rect.w = 0x100;
        p->rect.h = 1;
        /* The descriptor's x/y/w/h prefix is the RECT LoadImage2 takes. */
        LoadImage2(&p->rect, (u32 *)image_buffer);
        *(s32 *)&p->alternate_sector_destination = (s32)gFile_StagingBuffer.bytes;
        *(s32 *)&p->sector_destination = (s32)gFile_StagingBuffer.bytes;
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
        destination = gRuntime_pSharedWorkBase;
        v_3 &= m;
        gFile_dwTransferState = v_3;
        p->alternate_sector_destination = destination;
        p->sector_destination = destination;
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
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

#include "high_memory_addresses.h"
#include "shared_work_base.h"
#include "trig_constants.h"

/* The package's first sector in WA.MRG. A regional build defines its own. */
#ifndef CAMPAIGN_SCENE_PACKAGE_START_SECTOR
#define CAMPAIGN_SCENE_PACKAGE_START_SECTOR FILE_WA_CAMPAIGN_SCENE_START_SECTOR
#endif

#ifndef CAMPAIGN_SCENE_PACKAGE_SECTOR_COUNT
#define CAMPAIGN_SCENE_PACKAGE_SECTOR_COUNT FILE_WA_CAMPAIGN_SCENE_SECTOR_COUNT
#endif

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
 * a reference to the slot pointer and that changes which pseudo the allocator ranks
 * first: the %hi temporary for gSceneScript_Slots then takes $v0 and the address is
 * completed as `addiu $s0,$v0`, where retail coalesces the pair into
 * `lui $s0` / `addiu $s0,$s0` and emits it after the callback address rather
 * than before. Subscripting drops that reference and the prologue falls into
 * retail's order. */
void Campaign_LoadScenePackage(s16 scene_index)
{
    SceneScriptSlotTable *table;
    CampaignDialogPortrait *src;
    RECT *second;
    s32 i;

    table = &gSceneScript_Slots;
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

    table->images.image_id = -1;
    for (i = 0; i < 3; i++) {
        table->slots[i].object = 0;
        table->slots[i].callback.control = 0;
    }
    File_WaitForTransfers();

    while (IsIdleGPU(10)) {
        ;
    }

    /* Stage 3 of the package load fills the shared arena with the portraits. */
    src = (CampaignDialogPortrait *)gRuntime_pSharedWorkBase;
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
        LoadImage2(&gGraphics_aImageTransferRects[0], src->image);
        LoadImage2(&gGraphics_aImageTransferRects[1], src->clut);
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
        src++;
    } while (i < CAMPAIGN_DIALOG_PORTRAIT_COUNT);

    gCampaign_PrimaryDisplayObject = Campaign_CreatePrimaryDisplayObject();
}

/* Initialized data this unit owns at 0x80090CAC: the two callbacks SceneScript_UpdateSlots selects
 * from each live scene-script record's byte at +4. Both take the record and
 * its display object, the two arguments SceneScript_UpdateSlots passes.
 */

SceneScriptRecordCallback gSceneScript_RecordCallbacks[] = {
    SceneScript_NoOpRecordCallback,
    SceneScript_UpdateColorPulse,
};

/* The two scene-script record callbacks, in the order gSceneScript_RecordCallbacks
   (this unit) holds them: SceneScript_UpdateSlots picks
   one by each live record's byte at +4. */

void SceneScript_NoOpRecordCallback(SceneScriptSlot *state, DisplayObject *object)
{
}

void SceneScript_UpdateColorPulse(SceneScriptSlot *state, DisplayObject *color)
{
    s32 angle;
    s32 intensity;
    s32 component;
    DisplayObject *object;

    if (((u16)state->callback.control & 0x8000) == 0) {
        state->callback.control |= 0x8000;
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
    color->rgb.c.b = component;
    color->rgb.c.g = component;
    color->rgb.c.r = component;
    object = gSceneScript_Slots.slots[0].object;
    if (object != 0) {
        component = (intensity + 24) / 2 - 128;
        object->rgb.c.b = component;
        object->rgb.c.g = component;
        object->rgb.c.r = component;
    }
    object = gSceneScript_Slots.slots[1].object;
    if (object != 0) {
        component = (intensity + 24) / 2 - 128;
        object->rgb.c.b = component;
        object->rgb.c.g = component;
        object->rgb.c.r = component;
    }
}

void SceneScript_UpdateSlots(SceneScriptSlot *records) {
    s32 i;
    for (i = 0; i < 3; i++) {
        DisplayObject *value = records->object;
        if (value != 0) {
            gSceneScript_RecordCallbacks[records->callback.index](records, value);
        }
        records++;
    }
}

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
