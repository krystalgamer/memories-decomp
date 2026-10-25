#include "../types.h"
#define DUEL_FIELD_GRID_2D
#define GDIALOG_CHOICE_IN_DATA
#define GINPUT_PAD1_PRESSED_IN_DATA
#define GDUEL_WSELECTEDCARDID_IN_DATA
#define D_8009B360_AS_SIDE_ARRAY
#define DUEL_CARD_STAGING_DECK_VIEW
#define DISPLAY_OBJECT_WORK_RITUAL_VIEW
#define DISPLAY_OBJECT_MOTION_BYTE_CALLBACK
#include "ai.h"
#include "duel_side_state.h"
#include "ai_controller_handoff.h"
#include "duel_grid.h"
#include "../psyq/stdio.h"
#include "card_constants.h"
#include "duel_action_lock.h"
#include "duel_effect_request.h"
#include "duel_card.h"
#include "duel_card_record_lifecycle.h"
#include "duel_effect_tables.h"
#include "duel_magic_effect_format.h"
#include "duel_card_checks.h"
#include "duel_field_effects.h"
#include "../unmatched.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "display_object_helpers.h"
#include "sound.h"
#include "duel_card_effects.h"
#include "duel_swords_effect.h"
#include "duel_check_ritual.h"
#include "duel_ritual_controller.h"
#include "duel_card_display_state.h"
#include "duel_card_staging.h"
#include "duel_deck_card.h"
#include "display_object_work_slots.h"
#include "display_object_interpolation.h"
#include "display_object_motion.h"
#include "display_object_core.h"
#include "duel_card_capture_rounded_texture.h"
#include "duel_create_card_effect_overlay.h"
#include "card_preview_objects.h"
#include "duel_effect_allocate_request.h"
#include "text_box_lifecycle.h"
#include "dialog_read_choice_input.h"
#include "dialog_choice.h"
#include "input.h"
#include "text_box_update_channels.h"
#include "../psyq/rand.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "duel_effect_resource_setup.h"
#include "duel_effect_resource_record.h"
#define gDuel_sEffectWorkCounter D_8009B20C[1]

void DuelEffect_StartSwords(void)
{
    DuelEffectRequest *object;

    if (DuelEffect_MarkInitialized() == 0) {
        object = DuelEffect_CreateRequest(0x12);
        object->field_00 = 0xA0;
        object->field_02 = 0x78;
        object->phase = 1;
        SD_SEPlayFull(2);
    } else {
        gDuel_wCardEffectFlags = 0;
    }
}

/* Applies Swords of Revealing Light to the opposing side. The handler creates
 * its persistent effect object, stages the occupied field objects, then
 * installs the three-turn attack lock once the presentation completes. */

#define DUEL_FIELD_EFFECT_OBJECT_VIEW(object) \
    ((DuelFieldEffectObject *)(object))

void DuelEffect_ApplySwords(void)
{
    DuelEffectRequest *object;
    DuelCardRecord *entry;
    int slot;
    int side;
    int field_side;

    if (DuelEffect_MarkInitialized() == 0) {
        object = DuelEffect_AllocateRequest(0x15);
        side = DUEL_SIDE_SELECTOR_VOLATILE ^ 1;
        gDuelEffect_pTrackedRequest = object;
        gDuel_apSwordsEffectObjects[side] = object;
        object->flags |= DUEL_EFFECT_REQUEST_FLAG_NONBLOCKING;
        object->field_00 = 0xA0;
        object->field_02 = 0x78;
        field_side = DUEL_SIDE_SELECTOR_VOLATILE ^ 1;
        object->phase = field_side;
        SD_SEPlayFull(0x23);
    } else if (!(gDuel_wCardEffectFlags & 0x40)) {
        if (gDuelEffect_pTrackedRequest->progress != 0) {
            gDuel_wCardEffectFlags |= 0x40;
            for (
                slot = DUEL_FIELD_ROW_SIZE;
                slot < DUEL_FIELD_SIDE_ZONE_COUNT;
                slot++
            ) {
                entry = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices[DUEL_SIDE_SELECTOR_VOLATILE][slot]];
                if ((*(u32 *)&entry->terrain_modifier & 0x90000000) ==
                    0x90000000) {
                    DuelFieldEffectObject *current;

                    current = DUEL_FIELD_EFFECT_OBJECT_VIEW(entry->object);
                    current->callback = DuelEffect_UpdateRevealCard;
                    current->active = 1;
                }
            }
        }
    } else if (DisplayObject_FindAllocatedByTag(1) == 0 &&
               gDuelEffect_pTrackedRequest->progress >= 2) {
        gDuel_aSideStates[DUEL_SIDE_SELECTOR_VOLATILE ^ 1].swords_turns_remaining =
            DUEL_SWORDS_INITIAL_COUNTER;
        gDuel_wCardEffectFlags = 0;
    }
}

/* One step of a field-wide effect sweep, driven once per 16 frames by the
 * gDuel_wStepCounter countdown. The first entry resets the step index in
 * gDuel_sEffectWorkCounter; every later entry advances it and spawns a type-8 effect
 * object over the next slot of the third grid row (slots 10..14 of the acting
 * side), offsetting the object's depth by the step so the objects stagger.
 * Steps past the fifth clear gDuel_wCardEffectFlags and end the sweep. A card sitting in
 * the swept slot with a negative stat modifier has it cleared and gets the
 * alternate object state 5. */
void DuelEffect_ApplyCursebreaker(void) {
    DuelCardRecord *record;
    DuelEffectRequest *object;
    DuelFieldPosition *position;
    u8 *positions;
    s32 card;
    s32 next;
    s32 base_slot;
    s32 x;
    s32 timer;
    s32 step;
    s32 position_index;
    u8 *grid;

    if (DuelEffect_MarkInitialized() == 0) {
        gDuel_sEffectWorkCounter = -1;
        gDuel_wStepCounter = 0;
    }

    timer = gDuel_wStepCounter - 1;
    gDuel_wStepCounter = timer;
    if ((s16)timer > 0) {
        return;
    }

    gDuel_wStepCounter = 0x10;
    next = *(u16 *)&gDuel_sEffectWorkCounter + 1;
    gDuel_sEffectWorkCounter = next;
    if ((s16)next < DUEL_FIELD_ROW_SIZE) {
        /* This step walks the grid flat, with the side folded into base_slot
           below, while sibling DuelEffect_ApplySwords uses the two-dimensional view
           selected by DUEL_FIELD_GRID_2D. The cast is the one
           place the two spellings meet. The single-pass loop preserves the
           retail grid and side register allocation under GCC 2.8.1. The
           selected record uses DuelFieldPosition, with u16 member views to
           retain retail's unsigned halfword loads. */
        do {
            grid = (u8 *)gDuel_abFieldGridCardRecordIndices;
            base_slot = DUEL_SIDE_SELECTOR_VOLATILE * DUEL_FIELD_SIDE_GRID_SLOT_COUNT +
                        DUEL_FIELD_SIDE_ZONE_COUNT;
        } while ((s16)next == 0 && (s16)next != 0);
        card = grid[(s16)next + base_slot];
        record = &gDuel_aCardRecords[card];
        object = DuelEffect_AllocateRequest(8);
        positions = (u8 *)gDuel_aFieldGridGroundPositions;
        step = gDuel_sEffectWorkCounter;
        /* Keep the row bias in the typed index for retail scheduling. */
        position_index = step + DUEL_FIELD_SIDE_ZONE_COUNT;
        object->phase = 3;
        position = (DuelFieldPosition *)(
            (u32)&((DuelFieldPosition *)0)[position_index] +
            DUEL_SIDE_SELECTOR_VOLATILE * DUEL_FIELD_SIDE_POSITION_BYTES +
            positions
        );
        x = *(u16 *)&position->x;
        object->field_02 = 0;
        object->field_00 = (s16)x;
        object->field_04 = (s16)*(u16 *)&position->y;
        object->buffer = (void *)((s32)object->buffer + step * 0x3000);
        SD_SEPlayFull(0x14);

        if ((record->flags & DUEL_CARD_FLAG_OCCUPIED) != 0) {
            if (record->stat_modifier < 0) {
                record->stat_modifier = 0;
                object->phase = 5;
            }
        }
    } else {
        gDuel_wCardEffectFlags = 0;
    }
}

void DuelEffect_StartRitual(void)
{
    if (!DuelEffect_MarkInitialized()) {
        gDuel_RitualResultCardId = Duel_CheckRitual(0, gDuel_wEffectCardID);
        if (gDuel_RitualResultCardId) {
            DuelEffectRequest *request = DuelEffect_CreateRequest(0x12);

            request->field_00 = 0xA0;
            request->field_02 = 0x78;
            request->phase = 1;
            SD_SEPlayFull(2);
            CardPreview_RequestResourceTransfer(1, gDuel_RitualResultCardId);
        }
    } else if (!(
        (gFile_dwTransferState_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
        gFile_dwCancelRequestState_abs
    )) {
        gDuel_wCardEffectFlags = 0;
    }
}

/* Regional values: the ritual card's slide-in start and rest y (-34/22 ->
   -42/14) and the screen height it slides from (240 -> 256). The European
   build (src/game/european/) defines its own. */
#ifndef DUEL_RITUAL_CARD_START_Y
#define DUEL_RITUAL_CARD_START_Y -34
#endif
#ifndef DUEL_RITUAL_CARD_REST_Y
#define DUEL_RITUAL_CARD_REST_Y 22
#endif
#ifndef DUEL_RITUAL_SCREEN_HEIGHT
#define DUEL_RITUAL_SCREEN_HEIGHT 240
#endif

#ifndef DUEL_EFFECT_RITUAL_CONFIRM_MASK
#define DUEL_EFFECT_RITUAL_CONFIRM_MASK PAD_BUTTON_CONFIRM_MASK
#endif

void DuelEffect_ApplyRitual(void)
{
    DisplayObject *object;
    DuelCardRecord *card;
    DuelEffectChannel *text;
    s32 value;
    s32 timer;
    u16 flags;

    if (!DuelEffect_MarkInitialized()) {
        gDuel_RitualResultCardId = Duel_CheckRitual(
            &gDisplayObject_apWorkSlots.ritual.result, gDuel_wEffectCardID);
        if (gDuel_RitualResultCardId) {
            Duel_MarkCreatedCardKnown((s32)gDuel_RitualResultCardId);
            gDuelEffect_pTrackedRequest = DuelEffect_AllocateRequest(22);
            gDuel_wTrapPresentationMode = 0;
            gDuelEffect_pTrackedRequest->phase = gDuel_wEffectCardID;
            SD_CdSoundPlay(0x8022);
        } else {
            goto done;
        }
        return;
    }
    switch (gDuel_wTrapPresentationMode & 0xF) {
    case 0:
        if (gDuelEffect_pTrackedRequest->progress) {
            gDuel_bCardMoveSlot = gDisplayObject_apWorkSlots.slots[3]->field_6A;
            DuelCard_DeactivateRecord(&gDuel_aCardRecords[gDisplayObject_apWorkSlots.slots[2]->field_6A]);
            DuelCard_DeactivateRecord(&gDuel_aCardRecords[gDisplayObject_apWorkSlots.slots[3]->field_6A]);
            DuelCard_DeactivateRecord(&gDuel_aCardRecords[gDisplayObject_apWorkSlots.slots[4]->field_6A]);
            gDuel_wTrapPresentationMode = 1;
        }
        break;
    case 1: {
        u16 scale;
        s32 countdown;
        object = gDuel_pCardPreviewObject;
        if (!(gDuel_wTrapPresentationMode & 0x80)) {
            s32 attribute;
            s16 object_flags;
            gDuel_wTrapPresentationMode |= 0x80;
            object = (DisplayObject *)CardPreview_CreateResourceObjects(1, -1, -1);
            object->field_30.h.field_30 = 90;
            /* Negative constants go into these u16 halves through an s16
               view here and below; stored as u16, GCC materialises the
               zero-extended value (li 0xffde) where retail has li -34. */
            *(s16 *)&object->field_30.h.field_32 = DUEL_RITUAL_CARD_START_Y;
            object->field_60 = 24;
            attribute = object->attribute;
            object_flags = object->flags;
            object->field_44.h.field_46 = 0;
            object->field_44.h.field_44 = 0;
            object->attribute = attribute & 0xF7FFFFFF;
            object->flags = object_flags | 4;
            DisplayObject_ResetVelocity((DisplayObjectVelocity *)object);
            object->field_38.h.field_38 = 597;
            gDuel_pCardPreviewObject = object;
            SD_SEPlayFull(0x26);
        }
        DisplayObject_StepPositionY((DisplayObjectVelocity *)object);
        scale = (u16)object->field_44.h.field_44 + 170;
        countdown = (u16)object->field_60 - 1;
        object->field_44.h.field_44 = scale;
        object->field_44.h.field_46 = scale;
        object->field_60 = countdown;
        if ((s32)((u32)countdown << 16) <= 0) {
            s32 attribute;
            s16 object_flags;
            object->field_44.h.field_46 = 4096;
            object->field_44.h.field_44 = 4096;
            object->field_30.h.field_32 = DUEL_RITUAL_CARD_REST_Y;
            gDuel_wTrapPresentationMode = 2;
            attribute = object->attribute | 0x08000000;
            object_flags = object->flags & 0xFFFB;
            object->attribute = attribute;
            object->flags = object_flags;
        }
        break;
    }
    case 2:
        object = gDuel_pCardPreviewObject;
        if (!(gDuel_wTrapPresentationMode & 0x80)) {
            gDuel_wTrapPresentationMode |= 0x80;
            gDuel_sEffectWorkCounter = 32;
        }
        if (!(gDuel_wTrapPresentationMode & 0x40)) {
            timer = *(u16 *)&gDuel_sEffectWorkCounter - 1;
            gDuel_sEffectWorkCounter = timer;
            if ((s32)((u32)timer << 16) <= 0) {
                gDuel_wTrapPresentationMode |= 0x40;
                DuelCard_CaptureRoundedTexture(object);
                gDisplayObject_apWorkSlots.slots[0] =
                    Duel_CreateCardEffectOverlay(
                        (DisplayObjectConfigView *)object);
                gDisplayObject_apWorkSlots.slots[0]->attribute |= 0x50000000;
                gDisplayObject_apWorkSlots.slots[0]->attribute &= ~0x08000000;
                gDisplayObject_apWorkSlots.slots[1] =
                    Duel_CreateCardEffectOverlay(
                        (DisplayObjectConfigView *)object);
                DisplayObject_SetDepthOffset(gDisplayObject_apWorkSlots.slots[1], -1);
                gDisplayObject_apWorkSlots.slots[1]->attribute |= 0x60000000;
                gDisplayObject_apWorkSlots.slots[1]->attribute &= ~0x08000000;
                DuelEffect_ReleaseResourceObjects(1);
            }
        } else {
            DisplayObject *first = gDisplayObject_apWorkSlots.slots[0];
            DisplayObject *second = gDisplayObject_apWorkSlots.slots[1];
            value = first->field_44.h.field_44 + 128;
            second->field_44.h.field_46 = value;
            second->field_44.h.field_44 = value;
            first->field_44.h.field_46 = value;
            first->field_44.h.field_44 = value;
            value = *(u8 *)&gDisplayObject_apWorkSlots.slots[0]->field_0C;
            value -= 4;
            if (value < 0)
                value = 0;
            value = value | ((value << 16) | (value << 8));
            gDisplayObject_apWorkSlots.slots[0]->field_0C = value;
            gDisplayObject_apWorkSlots.slots[1]->field_0C = value;
            if (!value) {
                DisplayObject_ReleaseIfPresent(gDisplayObject_apWorkSlots.slots[0]);
                DisplayObject_ReleaseIfPresent(gDisplayObject_apWorkSlots.slots[1]);
                gDuel_wTrapPresentationMode = 3;
            }
        }
        break;
    case 3: {
        u16 state = gDuel_wTrapPresentationMode;
        DuelEffectResourceRecord *resource = (DuelEffectResourceRecord *)gCardPreview_Resource1;
        u8 *data;
        card = &gDuel_aCardRecords[gDuel_bCardMoveSlot];
        if (!(state & 0x80)) {
            u16 ritual;
            u16 y;
            /* The data pointer is reloaded through its address as u8 **, a
               non-struct read; as a plain card->data read the function's
               schedule changes (17 differences). */
            ((DuelDeckCardRecord *)card->data)->id =
                (s16)(ritual = gDuel_RitualResultCardId);
            data = *(u8 **)&card->data;
            gDuel_wTrapPresentationMode = state | 0x80;
            card->card_id = ritual;
            value = ((DuelDeckCardRecord *)data)->data_block_index;
            y = resource->src_y;
            resource->rects[0].x = resource->src_x + 56;
            resource->rects[0].w = 8;
            resource->rects[0].h = 88;
            resource->rects[0].y = y;
            StoreImage(&resource->rects[0],
                (u32 *)&gDuel_aCardThumbnailBlocks[value]);
            break;
        }
        Duel_SetupFieldCardFromDeck(
            gDuel_bCardMoveSlot, ((DuelDeckCardRecord *)card->data)->deck_index);
        object = card->object;
        gDisplayObject_apWorkSlots.slots[0] = object;
        *(s16 *)&object->field_30.h.field_32 = -DUEL_RITUAL_SCREEN_HEIGHT;
        if (gDuel_bSide0ControllerSelector[gDuel_bActiveSide] >= 0) {
            card->flags &= ~0x200;
            if (rand() & 1)
                card->flags |= 0x200;
            goto state_five;
        }
        gDisplayObject_apWorkSlots.slots[1] =
            (DisplayObject *)Duel_CreateCardDisplayObject(card, 134, DUEL_RITUAL_SCREEN_HEIGHT);
        gDuel_wTrapPresentationMode = 4;
    }
    case 4:
        object = gDisplayObject_apWorkSlots.slots[1];
        if (!(gDuel_wTrapPresentationMode & 0x80)) {
            gDuel_wTrapPresentationMode |= 0x80;
            object->position.h.field_28 = 134;
            object->position.h.field_2A = 42;
            object->field_2C.h.field_2C = 16;
            object->field_6C = 1;
            object->update = DisplayObject_UpdateCosineSlide;
        } else if (!(gDuel_wTrapPresentationMode & 0x40)) {
            if (!DisplayObject_FindAllocatedByTag(1)) {
                gDuel_wSelectedCardID =
                    D_8015C424_cards.field_cards[object->field_6A].card_id;
#ifdef VERSION_JAPAN
                text = TextBox_CreateFlagged(0, 33, 80, 110, 160, 48, 32);
#elif defined(VERSION_EUROPE)
                /* The European box takes 64 in its last argument and keeps
                   the text size InitEntry sets. */
                text = TextBox_CreateFlagged(0, 33, 72, 110, 176, 48, 64);
#else
                text = TextBox_CreateFlagged(0, 33, 72, 110, 176, 48, 32);
                text->field_5A = 8;
                text->field_5B = 16;
#endif
                do {
                    TextBox_UpdateChannels();
                } while (!text->field_30);
                gDuel_wTrapPresentationMode |= 0x40;
            }
        } else if (!(gDuel_wTrapPresentationMode & 0x20)) {
            if (!Dialog_ReadChoiceInput(gText_aChannels) &&
                (gInput_wPad1Pressed & DUEL_EFFECT_RITUAL_CONFIRM_MASK)) {
                SD_SEPlayFull(7);
                object->position.h.field_28 = 134;
                *(s16 *)&object->position.h.field_2A = -128;
                object->field_2C.h.field_2C = 16;
                object->field_6C = 1;
                object->update = DisplayObject_UpdateCosineSlide;
                card = &gDuel_aCardRecords[object->field_6A];
                flags = card->flags & ~0x200;
                card->flags = flags;
                if (gDialog_bChoice)
                    card->flags = flags | 0x200;
                TextBox_Destroy(gText_aChannels);
                gDuel_wTrapPresentationMode |= 0x20;
            }
        } else if (!DisplayObject_FindAllocatedByTag(1)) {
            DisplayObject_ReleaseIfPresent(object);
state_five:
            gDuel_wTrapPresentationMode = 5;
        }
        break;
    case 5: {
        s16 phase;
        object = gDisplayObject_apWorkSlots.slots[0];
        if (!(gDuel_wTrapPresentationMode & 0x80)) {
            gDuel_wTrapPresentationMode |= 0x80;
            DisplayObject_SavePosition((DisplayObjectSnapshot *)object);
            object->field_60 = -1024;
        }
        Widget_SlideSine((DisplayObjectPosition *)object,
            (s16)object->field_30.h.field_30, -24, object->field_60);
        phase = (u16)object->field_60 + 42;
        object->field_60 = phase;
        if (phase >= 0) {
            *(s16 *)&object->field_30.h.field_32 = -24;
done:
            gDuel_wCardEffectFlags = 0;
        }
        break;
    }
    }
}

void DuelEffect_ApplyHarpiesFeatherDuster(void)
{
    int i;

    if (!DuelEffect_MarkInitialized()) {
        DuelEffectRequest *object = DuelEffect_CreateRequest(0x17);
        int side = gDuel_bActiveSide ^ 1;

        object->field_04 = gDuel_aFieldGridGroundPositions[side][0].y;
#if !defined(VERSION_EUROPE)
        printf(D_8009AF40, (short)object->field_04);
#endif
    } else {
        for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
            int position =
                i + gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
            DuelCardRecord *entry =
                &gDuel_aCardRecords[((u8 *)gDuel_abFieldGridCardRecordIndices)[position]];

            if (entry->flags & DUEL_CARD_FLAG_OCCUPIED)
                DuelCard_RemoveFromField(entry);
        }
        gDuel_wCardEffectFlags = 0;
    }
}

int DuelEffect_UpdateCardEffect(void)
{
    u16 flags = gDuel_wCardEffectFlags;

    if (flags & DUEL_CARD_EFFECT_FLAG_ACTIVE) {
        u8 *indices = gDuelEffect_abGroupByEffectId;
        DuelEffectHandler *callbacks;
        int index = indices[gDuel_sCardEffectIndex] * DUEL_CARD_EFFECT_HANDLERS_PER_GROUP;

        if (flags & DUEL_CARD_EFFECT_FLAG_SECOND_HANDLER)
            index++;
        callbacks = gDuelEffect_apfnGroupHandler;
        callbacks[index]();
    }
    return gDuel_wCardEffectFlags;
}

void DuelEffect_StartCardEffect(int value, int flag)
{
    int index;

    if (((unsigned)(value - DUEL_EFFECT_FIRST_BLOCK_CARD_ID) <
         DUEL_EFFECT_CARD_BLOCK_SIZE) ||
        ((unsigned)(value - DUEL_EFFECT_SECOND_BLOCK_CARD_ID) <
         DUEL_EFFECT_CARD_BLOCK_SIZE) ||
        value == DUEL_DARK_MAGIC_RITUAL_CARD_ID) {
        if (value < DUEL_EFFECT_SECOND_BLOCK_CARD_ID) {
            index = value - DUEL_EFFECT_FIRST_BLOCK_CARD_ID;
        } else {
            index = value -
                (DUEL_EFFECT_SECOND_BLOCK_CARD_ID -
                 DUEL_EFFECT_CARD_BLOCK_SIZE);
            if (value == DUEL_DARK_MAGIC_RITUAL_CARD_ID) {
                index = DUEL_DARK_MAGIC_RITUAL_EFFECT_INDEX;
            }
        }
        gDuel_sCardEffectIndex = index;
        gDuel_wEffectCardID = value;
        gDuel_wCardEffectFlags = DUEL_CARD_EFFECT_FLAG_ACTIVE;
        if (flag) {
            gDuel_wCardEffectFlags =
                DUEL_CARD_EFFECT_FLAG_ACTIVE |
                DUEL_CARD_EFFECT_FLAG_SECOND_HANDLER;
        }
    }
}

#define DUEL_CARD_RECORD_AT_OFFSET(records, offset) \
    ((DuelCardRecord *)((u8 *)(records) + (offset)))

s32 Duel_FindFreeFieldSlot(s32 arg0)
{
    s32 base;
    s32 off;
    DuelCardRecord *p;
    s32 i;

    do {
        base = gDuel_bActiveSide * DUEL_CARD_SIDE_RECORD_COUNT + arg0;
        off = base << 3;
        off -= base;
        off <<= 2;
        p = DUEL_CARD_RECORD_AT_OFFSET(gDuel_aCardRecords, off);
        for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
            off = p->flags & DUEL_CARD_FLAG_OCCUPIED;
            if (off == 0) {
                return gDuel_bActiveSide * DUEL_CARD_SIDE_RECORD_COUNT + arg0 + i;
            }
            p++;
        }
    } while (0);
    return -1;
}

s32 Duel_CollectFieldCardsBelowType(DuelCardRecord **out, s32 arg1,
                                    s32 arg2)
{
    s32 count = 0;
    s32 i = 0;
    s32 d = gDuel_bActiveSide;
    s32 *t = gDuel_adwCardStats;
    DuelCardRecord *r =
        &gDuel_aCardRecords[d * DUEL_CARD_SIDE_RECORD_COUNT + arg1];
    s32 k;

    do {
        if (r->flags & DUEL_CARD_FLAG_OCCUPIED) {
            k = (s16)r->card_id;
            k--;
            if (((t[k] >> CARD_STAT_TYPE_SHIFT) &
                 CARD_STAT_TYPE_MASK) < arg2) {
                *out++ = r;
                count++;
            }
        }
        i++;
        r++;
    } while (i < DUEL_FIELD_ROW_SIZE);

    *out = 0;
    return count;
}

s32 Duel_CollectFieldCardsByType(DuelCardRecord **out, s32 arg1,
                                 s32 arg2) {
    s32 count = 0;
    s32 i = 0;
    s32 d = gDuel_bActiveSide;
    s32 *t = gDuel_adwCardStats;
    DuelCardRecord *r =
        &gDuel_aCardRecords[d * DUEL_CARD_SIDE_RECORD_COUNT + arg1];
    s32 k;

    do {
        /* The `do { } while (0)` is load-bearing and NOT a macro leftover:
         * it is 2 differences without it and 6 with `{ }` or `if (1) { }`
         * instead, because gcc's loop pass sees the extra loop node and
         * numbers the blocks differently. Coupled with `count` being
         * initialised before `i` -- that alone is 6. Found by the permuter;
         * see docs/PARKED.txt's former entry. */
        do {
            if (r->flags & DUEL_CARD_FLAG_OCCUPIED) {
                if (arg2 < 0 ||
                    (k = (s16)r->card_id, k--,
                     ((t[k] >> CARD_STAT_TYPE_SHIFT) &
                      CARD_STAT_TYPE_MASK) == arg2)) {
                    *out++ = r;
                    count++;
                }
            }
        } while (0);
        i++;
        r++;
    } while (i < DUEL_FIELD_ROW_SIZE);

    *out = 0;

    return count;
}

s32 Duel_SelectEquipPair(void) {
    DuelCardRecord *a[DUEL_FIELD_ROW_SIZE + 1];
    DuelCardRecord *b[DUEL_FIELD_ROW_SIZE + 1];
    DuelCardRecord **q;
    DuelCardRecord **r;
    DuelCardRecord *e;
    DuelCardRecord *f;
    s32 n;
    s32 u;
    s32 w;

    if (Duel_CollectFieldCardsByType(b, 0, CARD_TYPE_EQUIP) != 0) {
        Duel_CollectFieldCardsByType(a, DUEL_FIELD_ROW_SIZE, -1);
        e = b[0];
        if (e != 0) {
            r = b;
            do {
                f = a[0];
                if (f != 0) {
                    q = a;
                    do {
                        if (Duel_CheckEquip(e->card_id, f->card_id) != 0) {
                            gAi_Selection.bytes[0] =
                                *(s8 *)&e->table_index % DUEL_FIELD_ROW_SIZE + 0xB;
                            gAi_Selection.bytes[1] = 0;
                            gAi_Selection.bytes[6] =
                                *(s8 *)&f->table_index % DUEL_FIELD_ROW_SIZE + 1;
                            gAi_Selection.bytes[7] = 0;
                            gAi_Selection.bytes[8] = 0;
                            return 0;
                        }
                        q++;
                        f = *q;
                    } while (f != 0);
                }
                r++;
                e = *r;
            } while (e != 0);
        }

        n = Duel_FindFreeFieldSlot(DUEL_FIELD_ROW_SIZE);
        if (n < 0) {
            return 1;
        }
        Duel_CollectFieldCardsBelowType(a, 0, CARD_TYPE_MAGIC);
        e = b[0];
        if (e != 0) {
            r = b;
            do {
                f = a[0];
                if (f != 0) {
                    q = a;
                    do {
                        if (Duel_CheckEquip(e->card_id, f->card_id) != 0) {
                            u = *(s8 *)&f->table_index;
                            gAi_Selection.bytes[0] =
                                u % DUEL_FIELD_ROW_SIZE + 0xB;
                            w = *(s8 *)&e->table_index;
                            gAi_Selection.bytes[2] = 0;
                            gAi_Selection.bytes[7] = 0;
                            gAi_Selection.bytes[8] = 0;
                            gAi_Selection.bytes[6] =
                                n % DUEL_FIELD_ROW_SIZE + 1;
                            gAi_Selection.bytes[1] =
                                w % DUEL_FIELD_ROW_SIZE + 0xB;
                            return 0;
                        }
                        q++;
                        f = *q;
                    } while (f != 0);
                }
                r++;
                e = *r;
            } while (e != 0);
        }
    }
    return 1;
}

s32 Duel_SelectTrapPlay(void) {
    DuelCardRecord *sp10[6];
    s32 slot;
    s8 v;

    if (Duel_CollectFieldCardsByType(sp10, 0, CARD_TYPE_TRAP) == 0) {
        return 1;
    }
    slot = Duel_FindFreeFieldSlot(DUEL_FIELD_SIDE_ZONE_COUNT);
    if (slot < 0) {
        return 1;
    }
    v = sp10[0]->table_index;
    gAi_Selection.bytes[8] = 1;
    gAi_Selection.bytes[1] = 0;
    gAi_Selection.bytes[7] = 0;
    gAi_Selection.bytes[6] = slot % DUEL_FIELD_ROW_SIZE + 6;
    gAi_Selection.bytes[0] = v % DUEL_FIELD_ROW_SIZE + 0xB;
    return 0;
}
