#include "../types.h"
#include "../psyq/rand.h"
#include "display_object.h"
#include "display_object_core.h"
#include "display_object_helpers.h"
#include "display_object_interpolation.h"
#include "display_object_work_slots.h"
#include "duel_scene_state.h"
#include "duel_selection_layout.h"
#include "duel_field_display_objects.h"
#include "duel_side_state.h"
#include "duel_hand.h"
#include "duel_card.h"
#include "duel_card_display_state.h"
#include "duel_card_record_lifecycle.h"
#include "duel_card_checks.h"
#include "duel_deck_card.h"
#define DUEL_CARD_STAGING_DECK_VIEW
#include "duel_card_staging.h"
#include "duel_action_lock.h"
#include "duel_effect_request.h"
#include "duel_effect_resource_record.h"
#include "duel_effect_resource_setup.h"
#include "duel_ritual_controller.h"
#include "display_object_start_alpha_transition.h"
#include "card_preview_objects.h"
#include "text_box_lifecycle.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "sound.h"
#include "../unmatched.h"
#include "duel_field_card_render_data.h"
#include "duel_scene_card_placement.h"
#include "duel_presentation_controls.h"
#include "display_callback_transitions.h"

/* Regional value: the screen height a card leaves at (240 -> 256). The
 * European build (src/game/european/) defines its own. */
#ifndef DUEL_CARD_PLACEMENT_SCREEN_HEIGHT
#define DUEL_CARD_PLACEMENT_SCREEN_HEIGHT 240
#endif

#define PLACEMENT_PX(object) ((object)->field_30.h.field_30)
#define PLACEMENT_PY(object) ((object)->field_30.h.field_32)
#define PLACEMENT_TX(object) ((object)->position.h.field_28)
#define PLACEMENT_TY(object) ((object)->position.h.field_2A)
#define PLACEMENT_VX(object) ((object)->field_34.h.field_36)
#define PLACEMENT_VY(object) ((object)->field_38.h.field_38)
#define PLACEMENT_ALPHA(object) ((object)->field_20.b.rotation_y)
#define PLACEMENT_ROTATION(object) ((object)->field_20.b.rotation_z)
#define PLACEMENT_REPLAY_CARD(object) (D_8015C424_cards.field_cards[(object)->field_6A])
#define DISPLAY_OBJECT_VELOCITY_VIEW(object) \
    ((DisplayObjectVelocity *)(object))

s16 gDuel_wEquipCardId;

static __inline__ s32 placement_velocity(s32 distance)
{
    return distance / 8;
}

void DuelScene_UpdateCardPlacement(void)
{
    DuelSelectionRecord *side = (DuelSelectionRecord *)
        (gDuel_abSelectionSides + gDuel_bActiveSide * DUEL_SELECTION_SIDE_SIZE);
    DisplayObject *object;
    DuelCardRecord *card;
    DuelEffectRequest *effect;
    s32 n;
    s32 value;
    s32 phase_clear;
    s32 x;

    if (!(gDuel_wSceneStateFlags & 0x8000)) {
        DisplayObject *G32 *slots;
        DisplayObject *G32 *cursor;
        DuelHandSlot *hand;
        s16 y;
        x = 260;
        value = 4;
        gDuel_wSceneStateFlags |= 0x8000;
        gDuel_pActiveSelectionRecord = side;
        gDuel_bPlacementNeedsCardMoveSetup = 0;
        gDuel_bPresentationCursor = 0;
        gDuel_wPendingEquipBonus = 0;
        n = 5;
        y = 82;
        slots = gDisplayObject_apWorkSlots;
        cursor = slots + 5;
        do {
            object = *cursor;
            if (object) {
                PLACEMENT_TX(object) = x;
                PLACEMENT_TY(object) = y;
                DisplayObject_SetDepthOffset(object, value);
                x -= 16;
                value++;
            }
            n--;
            cursor--;
        } while (n > 0);
        object = gDisplayObject_apWorkSlots[0];
        PLACEMENT_TX(object) = 64;
        PLACEMENT_TY(object) = 82;
        DisplayObject_SetDepthOffset(object, value);
        if ((u8)(object->field_6A % 15) < 5)
        gDuel_bPlacementNeedsCardMoveSetup = 1;
        slots = gDisplayObject_apWorkSlots;
        if (!slots[1]) {
            if (slots[0]->field_68 < 20)
            gDuel_bPlacementNeedsCardMoveSetup = 1;
            gDuel_wPlacementCardId = PLACEMENT_REPLAY_CARD(object).card_id;
            PLACEMENT_TX(object) = 134;
            PLACEMENT_TY(object) = 42;
            gDuel_wSceneStateFlags |= 0x4000;
        } else {
            gDuel_pActiveSideState->rank.field_07++;
        }
        n = 0;
        do {
            hand = &gDuelHand_aSlots[n];
            if (hand->selection_order) {
                DisplayObject *child = hand->order_marker;
                hand->object = 0;
                DisplayObject_ReleaseIfPresent(child);
                hand->order_marker = 0;
            }
            n++;
        } while (n < 5);
        DisplayObject_ReleaseIfPresent(side->cursor_object);
        gDuel_wDisplayTransitionFlags = 8;
        side->cursor_object = 0;
        gDuel_bSceneSubStep = 1;
        return;
    }

    switch (gDuel_bSceneSubStep & 15) {
    case 1:
        {
            DisplayObject *G32 *slots;
            {
                DisplayObject *G32 *cursor;
                DuelCardRecord *cards;
                DisplayObjectCallback callback;
                s32 active;
                if (!(gDuel_bSceneSubStep & 0x80)) {
                    gDuel_bSceneSubStep |= 0x80;
                    if (gDuel_wSceneStateFlags & 0x4000)
                    return;
                    n = 5;
                    cards = gDuel_aCardRecords;
                    active = 1;
                    callback = (DisplayObjectCallback)Duel_UpdateCardMarkerRemovalTurn;
                    slots = gDisplayObject_apWorkSlots;
                    cursor = slots + 5;
                    do {
                        object = *cursor;
                        if (object && (cards[object->field_6A].flags & 0x2000)) {
                            object->field_6C = active;
                            object->update = callback;
                        }
                        n--;
                        cursor--;
                    } while (n >= 0);
                    return;
                }
                if (DisplayObject_FindAllocatedByTag(1))
                return;
                gDuel_bSceneSubStep = 2;
            }
        }
    case 2:
        {
            DisplayObject *G32 *slots;
            {
                DisplayObject *G32 *cursor;
                if (!(gDuel_bSceneSubStep & 0x80)) {
                    DisplayObject *G32 *cursor;
                    s32 active;
                    gDuel_bSceneSubStep |= 0x80;
                    n = 5;
                    active = 1;
                    slots = gDisplayObject_apWorkSlots;
                    cursor = slots + 5;
                    do {
                        object = *cursor;
                        if (object) {
                            object->field_6C = active;
                            DisplayObject_SavePosition((DisplayObjectSnapshot *)object);
                            object->field_60 = 0;
                        }
                        n--;
                        cursor--;
                    } while (n >= 0);
                }
                value = 0;
                n = 5;
                slots = gDisplayObject_apWorkSlots;
                cursor = slots + 5;
                do {
                    object = *cursor;
                    if (object && object->field_6C) {
                        value = 1;
                        if (!(gDuel_wSceneStateFlags & 0x4000) && PLACEMENT_ALPHA(object))
                        PLACEMENT_ALPHA(object) += 8;
                        if (PLACEMENT_ROTATION(object))
                        PLACEMENT_ROTATION(object) += 8;
                        DisplayObject_InterpolatePositionCosine((DisplayObjectPosition *)object,
                        (s16)PLACEMENT_TX(object), (s16)PLACEMENT_TY(object), object->field_60);
                        object->field_60 += 128;
                        if (object->field_60 >= 2048) {
                            object->field_6C = 0;
                            PLACEMENT_ROTATION(object) = 0;
                            if (!(gDuel_wSceneStateFlags & 0x4000))
                            PLACEMENT_ALPHA(object) = 0;
                            if (!PLACEMENT_ALPHA(object))
                            object->flags &= ~4;
                            PLACEMENT_PX(object) = PLACEMENT_TX(object);
                            PLACEMENT_PY(object) = PLACEMENT_TY(object);
                        }
                    }
                    n--;
                    cursor--;
                } while (n >= 0);
                if (!(gDuel_wDisplayTransitionFlags | value)) {
                    gDuel_bSceneSubStep = 3;
                    if (gDuel_wSceneStateFlags & 0x4000)
                    gDuel_wSceneStateFlags = 8;
                }
            }
            break;
        }
    case 3:
        {
            DisplayObject *G32 *slots;
            if (!(gDuel_bSceneSubStep & 0x80)) {
                gDuel_bSceneSubStep |= 0x80;
                gDuel_bPresentationCursor++;
                if ((s8)gDuel_bPresentationCursor >= 6) {
                    gDuel_bSceneSubStep = 8;
                    return;
                }
                {
                    DisplayObject *selected = gDisplayObject_apWorkSlots[(s8)gDuel_bPresentationCursor];
                    if (!selected) {
                        gDuel_bSceneSubStep = 8;
                        return;
                    }
                    object = selected;
                }
                gDisplayObject_apWorkSlots[1] = object;
                DisplayObject_ResetVelocity(
                    DISPLAY_OBJECT_VELOCITY_VIEW(object));
                object->field_60 = 8;
                PLACEMENT_VX(object) = placement_velocity((64 - (s16)PLACEMENT_PX(object)) * 256);
                PLACEMENT_VY(object) = placement_velocity((82 - (s16)PLACEMENT_PY(object)) * 256);
                DisplayObject_SetDepthOffset(gDisplayObject_apWorkSlots[0], (s8)((u8)object->depth_offset - 1));
                SD_SEPlayFull(10);
                gDuel_wPlacementCardId = 0;
                if (gDisplayObject_apWorkSlots[0]) {
                    s32 result = Duel_CheckFusion(PLACEMENT_REPLAY_CARD(gDisplayObject_apWorkSlots[0]).card_id,
                    PLACEMENT_REPLAY_CARD(object).card_id);
                    gDuel_wPlacementCardId = result;
                    if ((s16)result) {
                        gDuel_wPlacementCardId = result | 0x8000;
                    } else {
                        s32 equip_result = Duel_CheckEquip(PLACEMENT_REPLAY_CARD(object).card_id,
                        PLACEMENT_REPLAY_CARD(gDisplayObject_apWorkSlots[0]).card_id);
                        gDuel_wPlacementCardId = equip_result;
                        if ((s16)equip_result)
                        goto request_combination;
                        gDuel_wPlacementCardId = Duel_CheckEquip(PLACEMENT_REPLAY_CARD(gDisplayObject_apWorkSlots[0]).card_id,
                        PLACEMENT_REPLAY_CARD(object).card_id);
                    }
                    if (!((DuelPlacementCardIdCell *)&gDuel_wPlacementCardId)->value)
                    return;
request_combination:
                    CardPreview_RequestResourceTransfer(0, gDuel_wPlacementCardId & 0xFFF);
                }
                return;
            }
            slots = gDisplayObject_apWorkSlots;
            object = slots[1];
            DisplayObject_StepPositionX(
                DISPLAY_OBJECT_VELOCITY_VIEW(object));
            object->field_60--;
            if (object->field_60)
            return;
            gDuel_wTrapPresentationMode = 0;
            if (!slots[0]) {
                slots[0] = object;
                gDuel_bSceneSubStep = 3;
                return;
            }
            object->field_30.word = slots[0]->field_30.word;
            if (!((DuelPlacementCardIdCell *)&gDuel_wPlacementCardId)->value) {
                gDuel_bSceneSubStep = 7;
                gDuel_bPlacementNeedsCardMoveSetup = -1;
            } else if (((DuelPlacementCardIdCell *)&gDuel_wPlacementCardId)->value & 0x8000) {
                gDuel_bPlacementNeedsCardMoveSetup = 1;
                gDuel_wPendingEquipBonus = 0;
                gDuel_bSceneSubStep = 6;
                gDuel_pActiveSideState->rank.fusions_initiated++;
            } else {
                gDuel_bSceneSubStep = 5;
                gDuel_wPendingEquipBonus += 500;
                gDuel_pActiveSideState->rank.equips_used++;
            }
            break;
        }
    case 4:
        {
            DisplayObject **slots;
            if ((gFile_dwTransferState_abs & 0x2000030) | gFile_dwCancelRequestState_abs)
            return;
            if (!(gDuel_bSceneSubStep & 0x80)) {
                gDuel_bSceneSubStep |= 0x80;
                gDuel_wTrapPresentationMode = 0;
                return;
            }
            switch (gDuel_wTrapPresentationMode & 15) {
            case 0:
                {
                    DisplayObject *G32 *slots;
                    {
                        s32 card_index = gDisplayObject_apWorkSlots[0]->field_6A;
                        DuelEffectResourceRecord *resource = gCardPreview_Resources;
                        card = &gDuel_aCardRecords[card_index];
                        slots = gDisplayObject_apWorkSlots;
                        if (!(gDuel_wTrapPresentationMode & 0x80)) {
                            gDuel_wTrapPresentationMode |= 0x80;
                            if ((s16)gDuel_wPlacementCardId & 0x8000) {
                                ((DuelDeckCardRecord *)card->data)->id = gDuel_wPlacementCardId & 0xFFF;
                                gDuel_wTrapPresentationMode |= 0x40;
                                card->card_id = gDuel_wPlacementCardId & 0xFFF;
                                value = ((DuelDeckCardRecord *)card->data)->data_block_index;
                                resource->rects[0].x = resource->src_x + 56;
                                resource->rects[0].y = resource->src_y;
                                resource->rects[0].w = 8;
                                resource->rects[0].h = 88;
                                StoreImage2(&resource->rects[0], (u32 *)&gDuel_aCardThumbnailBlocks[value]);
                            }
                            return;
                        }
                        if (gDuel_wTrapPresentationMode & 0x40) {
                            Duel_SetupCardRecord(
                                card_index,
                                ((DuelDeckCardRecord *)card->data)->deck_index
                            );
                            DuelCard_RemoveFromField(card);
                        }
                        value = Duel_CalcCardStats(card);
                        object = (DisplayObject *)CardPreview_CreateResourceObjects(0, value & 0xFFFF, value >> 16);
                        PLACEMENT_PY(object) = 16;
                        PLACEMENT_PX(object) = PLACEMENT_PX(gDisplayObject_apWorkSlots[0]) - 44;
                        object->flags &= ~0x40;
                        gDuel_pCardPreviewObject = object;
                        DisplayObject_StartAlphaTransition(gDisplayObject_apWorkSlots[0], 0, 64, 8);
                        if (slots[1]) {
                            s32 second = (s16)PLACEMENT_PX(slots[1]);
                            s32 first = (s16)PLACEMENT_PX(gDisplayObject_apWorkSlots[0]);
                            second += 52;
                            PLACEMENT_PX(gDuel_pCardPreviewObject) = (first + second) / 2 - 70;
                            DisplayObject_StartAlphaTransition(slots[1], 0, 64, 8);
                        }
                        gDuel_wTrapPresentationMode = 1;
                    }
                    break;
                }
            case 1:
                {
                    DisplayObject *G32 *slots;
                    s32 alpha;
                    if (DisplayObject_FindAllocatedByTag(1))
                    return;
                    if (!(gDuel_wTrapPresentationMode & 0x80)) {
                        slots = gDisplayObject_apWorkSlots;
                        slots[0]->flags &= ~0x40;
                        gDuel_wTrapPresentationMode |= 0x80;
                        if (slots[1])
                        slots[1]->flags &= ~0x40;
                        PLACEMENT_ALPHA(gDuel_pCardPreviewObject) = 192;
                        gDuel_pCardPreviewObject->flags |= 0x44;
                        return;
                    }
                    alpha = PLACEMENT_ALPHA(gDuel_pCardPreviewObject) + 8;
                    PLACEMENT_ALPHA(gDuel_pCardPreviewObject) = alpha;
                    if ((s8)alpha >= 0) {
                        DisplayObject *entry = gDuel_pCardPreviewObject;
                        u16 flags = entry->flags;
                        PLACEMENT_ALPHA(entry) = 0;
                        gDuel_wTrapPresentationMode = 2;
                        entry->flags = flags & ~4;
                    }
                    break;
                }
            case 2:
                {
                    DisplayObject *G32 *slots;
                    card = &gDuel_aCardRecords[gDisplayObject_apWorkSlots[0]->field_6A];
                    object = gDuel_pCardPreviewObject;
                    if (!(gDuel_wTrapPresentationMode & 0x80)) {
                        gDuel_wTrapPresentationMode |= 0x80;
                        object->field_60 = 30;
                        if (!((s16)gDuel_wPlacementCardId & 0x8000)) {
                            PLACEMENT_TX(object) = 0;
                            PLACEMENT_TY(object) = 500;
                            gDuel_wTrapPresentationMode |= 0x40;
                            object->field_2C.h.field_2C = card->stat_modifier;
                            if (gDuel_wEquipCardId == 0x291) {
                                PLACEMENT_TY(object) = 1000;
                                gDuel_wPendingEquipBonus += 500;
                            }
                        }
                    }
                    if (gDuel_wTrapPresentationMode & 0x40) {
                        s32 below;
                        u16 limit;
                        PLACEMENT_TX(object) += 31;
                        below = (s16)PLACEMENT_TX(object) < (s16)PLACEMENT_TY(object);
                        limit = PLACEMENT_TY(object);
                        if (!below) {
                            gDuel_wTrapPresentationMode &= ~0x40;
                            PLACEMENT_TX(object) = limit;
                            object->field_60 = 16;
                        }
                        card->stat_modifier = object->field_2C.h.field_2C + PLACEMENT_TX(object);
                        value = Duel_CalcCardStats(card);
                        gCardPreview_Resources[0].attack = value;
                        gCardPreview_Resources[0].defense = value >> 16;
                        return;
                    }
                    object->field_60--;
                    if (object->field_60 > 0)
                    return;
                    {
                        DisplayObject *entry = gDuel_pCardPreviewObject;
                        *(u16 *)((u32)entry + (u32)&((DisplayObject *)0)->flags) = entry->flags | 4;
                    }
                    slots = gDisplayObject_apWorkSlots;
                    DisplayObject_ReleaseIfPresent(slots[0]);
                    DisplayObject_ReleaseIfPresent(slots[1]);
                    slots[1] = 0;
                    slots[0] = 0;
                    object = (DisplayObject *)Duel_CreateCardDisplayObject(card, 64, 82);
                    gDisplayObject_apWorkSlots[0] = object;
                    object->field_20.word = 0;
                    gDuel_wTrapPresentationMode = 3;
                    object->flags &= ~0x44;
                    break;
                }
            case 3:
                {
                    DisplayObject **slots;
                    s32 alpha;
                    object = gDisplayObject_apWorkSlots[0];
                    if (!(gDuel_wTrapPresentationMode & 0x80)) {
                        alpha = PLACEMENT_ALPHA(gDuel_pCardPreviewObject) + 8;
                        PLACEMENT_ALPHA(gDuel_pCardPreviewObject) = alpha;
                        if ((u8)alpha < 64)
                        return;
                        {
                            u16 position = PLACEMENT_PX(gDuel_pCardPreviewObject);
                            gDuel_wTrapPresentationMode |= 0x80;
                            PLACEMENT_PX(object) = position + 44;
                        }
                        DisplayObject_StartAlphaTransition(object, 192, 0, 8);
                        object->flags |= 0x40;
                        DuelEffect_ReleaseResourceObjects(0);
                        return;
                    }
                    if (!DisplayObject_FindAllocatedByTag(1))
                    gDuel_wTrapPresentationMode = 4;
                    break;
                }
            case 4:
                {
                    DisplayObject **slots;
                    object = gDisplayObject_apWorkSlots[0];
                    if (!(gDuel_wTrapPresentationMode & 0x80)) {
                        gDuel_wTrapPresentationMode |= 0x80;
                        DisplayObject_SavePosition((DisplayObjectSnapshot *)object);
                        object->field_60 = 0;
                    }
                    DisplayObject_InterpolatePositionCosine((DisplayObjectPosition *)object, 64, 82, object->field_60);
                    object->field_60 += 128;
                    if (object->field_60 >= 2048)
                    gDuel_bSceneSubStep = 3;
                    break;
                }
            }
            break;
        }
    case 5:
        {
            DisplayObject **slots;
            if (!(gDuel_bSceneSubStep & 0x80)) {
                DisplayObject *G32 *slots;
                slots = gDisplayObject_apWorkSlots;
                slots[0]->flags &= ~0x40;
                gDuel_bSceneSubStep |= 0xC0;
                DisplayObject_StartAlphaTransition(slots[1], 0, 64, 8);
                object = gDisplayObject_apWorkSlots[0];
                if (object->field_68 != 23)
                object = slots[1];
                gDuel_wEquipCardId = PLACEMENT_REPLAY_CARD(object).card_id;
                return;
            }
            if (gDuel_bSceneSubStep & 0x40) {
                DisplayObject **slots;
                if (DisplayObject_FindAllocatedByTag(1))
                return;
                {
                    s32 phase = gDuel_bSceneSubStep;
                    if (phase & 0x20) {
                        phase_clear = phase & 0x9F;
                        goto finish_effect_step;
                    }
                }
                {
                    DisplayObject *G32 *release_slots = gDisplayObject_apWorkSlots;
                    gDuel_bSceneSubStep |= 0x20;
                    if (release_slots[1]->field_68 < 20) {
                        DisplayObject_ReleaseIfPresent(gDisplayObject_apWorkSlots[0]);
                        gDisplayObject_apWorkSlots[0] = release_slots[1];
                    } else {
                        DisplayObject_ReleaseIfPresent(release_slots[1]);
                    }
                }
                {
                    DisplayObject *G32 *publish_slots = gDisplayObject_apWorkSlots;
                    publish_slots[1] = 0;
                    gDisplayObject_apWorkSlots[0]->flags |= 0x40;
                    DisplayObject_StartAlphaTransition(gDisplayObject_apWorkSlots[0], 192, 0, 8);
                }
                return;
            }
            if (!(gDuel_bSceneSubStep & 0x20)) {
                DisplayObject **slots;
                gDuel_bSceneSubStep |= 0x20;
                effect = DuelEffect_AllocateRequest(
                    ((u32)((u16)gDuel_wEquipCardId - 301) < 2) * 4);
                effect->field_00 = PLACEMENT_PX(gDisplayObject_apWorkSlots[0]) + 26;
                gDuelEffect_pTrackedRequest = effect;
                effect->field_02 = PLACEMENT_PY(gDisplayObject_apWorkSlots[0]) + 30;
                if (gDuel_wEquipCardId == 302)
                effect->phase = 1;
                if (gDuel_wEquipCardId == 0x291)
                effect->phase = 10;
                SD_SEPlayFull(22);
            } else if (gDuelEffect_pTrackedRequest->progress) {
                gDuel_bSceneSubStep = 4;
            }
            break;
        }
    case 6:
        {
            DisplayObject *G32 *slots;
            if (!(gDuel_bSceneSubStep & 0x80)) {
                DisplayObject *G32 *slots;
                gDuel_bSceneSubStep |= 0xC0;
                Duel_MarkCreatedCardKnown((u32)(gDuel_wPlacementCardId & 0xFFF));
                slots = gDisplayObject_apWorkSlots;
                slots[0]->flags &= ~0x40;
                DisplayObject_StartAlphaTransition(slots[1], 0, 64, 8);
            }
            if (gDuel_bSceneSubStep & 0x40) {
                DisplayObject *G32 *slots;
                if (DisplayObject_FindAllocatedByTag(1))
                return;
                {
                    s32 phase = gDuel_bSceneSubStep;
                    if (phase & 0x20) {
                        phase_clear = phase & 0x9F;
                        goto finish_effect_step;
                    }
                }
                slots = gDisplayObject_apWorkSlots;
                gDuel_bSceneSubStep |= 0x20;
                DisplayObject_StartAlphaTransition(slots[1], 192, 0, 8);
                PLACEMENT_PX(slots[1]) = 140;
                gDisplayObject_apWorkSlots[0]->flags |= 0x40;
                DisplayObject_StartAlphaTransition(gDisplayObject_apWorkSlots[0], 192, 0, 8);
                return;
finish_effect_step:
                gDuel_bSceneSubStep = phase_clear;
                return;
            }
            if (!(gDuel_bSceneSubStep & 0x20)) {
                DisplayObject *G32 *slots;
                gDisplayObject_apWorkSlots[0]->field_60 = 0;
                PLACEMENT_TX(gDisplayObject_apWorkSlots[0]) = 1;
                PLACEMENT_TY(gDisplayObject_apWorkSlots[0]) = 0x2680;
                gDuel_bSceneSubStep |= 0x20;
                gDisplayObject_apWorkSlots[0]->field_2C.h.field_2E = 1;
                {
                    DuelEffectRequest *request;
                    request = DuelEffect_AllocateRequest(1);
                    request->field_00 = 128;
                    (*(s16 *)((u32)request + (u32)&((DuelEffectRequest *)0)->field_02)) = PLACEMENT_PY(gDisplayObject_apWorkSlots[0]) + 30;
                    gDuelEffect_pTrackedRequest = request;
                    slots = gDisplayObject_apWorkSlots;
                    if (slots[0]->field_68 >= 20 || slots[1]->field_68 >= 20)
                    request->phase = 1;
                }
                SD_SEPlayFull(24);
                return;
            }
            if (gDuelEffect_pTrackedRequest->progress) {
                DisplayObject *G32 *slots;
                slots = gDisplayObject_apWorkSlots;
                slots[0]->flags &= ~0x40;
                slots[1]->flags &= ~0x40;
                SD_SEPlayFull(25);
                gDuel_bSceneSubStep = 4;
            } else {
                object = gDisplayObject_apWorkSlots[0];
                object->field_2C.h.field_2E += 8;
                object->field_60 += gDisplayObject_apWorkSlots[0]->field_2C.h.field_2E;
                PLACEMENT_TY(object) -= object->field_60 / 12;
                n = (s16)PLACEMENT_TY(object) >> 8;
                x = rcos(object->field_60) * n / 4096;
                value = rsin(object->field_60) * n / 4096;
                PLACEMENT_PX(object) = 102 - x;
                PLACEMENT_PY(object) = 82 - value;
                slots = gDisplayObject_apWorkSlots;
                PLACEMENT_PX(slots[1]) = x + 102;
                PLACEMENT_PY(slots[1]) = value + 82;
            }
            break;
        }
    case 7:
        {
            DisplayObject *G32 *slots;
            slots = gDisplayObject_apWorkSlots;
            object = slots[0];
            if (!(gDuel_bSceneSubStep & 0x80)) {
                gDuel_bSceneSubStep |= 0x80;
                object->flags |= 4;
                object->attribute &= ~0x08000000;
                SD_SEPlayFull(26);
                if (slots[1]->field_68 >= 20 && object->field_68 < 20) {
                    DisplayObject *other = slots[1];
                    slots[1] = object;
                    object = other;
                    slots[0] = other;
                }
                DisplayObject_ResetVelocity(
                    DISPLAY_OBJECT_VELOCITY_VIEW(object));
                PLACEMENT_VX(object) = -((rand() & 255) + 768);
                PLACEMENT_VY(object) = -640;
            }
            PLACEMENT_VX(object) = DisplayObject_StepTowardZero(PLACEMENT_VX(object), 8);
            PLACEMENT_VY(object) = DisplayObject_StepToward(PLACEMENT_VY(object), 2048, 96);
            DisplayObject_StepPositionXY(
                DISPLAY_OBJECT_VELOCITY_VIEW(object));
            if ((s16)PLACEMENT_PX(object) < -52 || (s16)PLACEMENT_PY(object) >= DUEL_CARD_PLACEMENT_SCREEN_HEIGHT) {
                DisplayObject_ReleaseIfPresent(object);
                gDisplayObject_apWorkSlots[0] = gDisplayObject_apWorkSlots[1];
                TextBox_Destroy(gText_aChannels);
                gDuel_bSceneSubStep = 3;
            }
            break;
        }
    case 8:
        {
            DisplayObject *object;
            DuelCardRecord *card;
            DisplayObject **slots;
            object = gDisplayObject_apWorkSlots[0];
            gDuel_wSceneStateFlags = 6;
            if (object->field_68 < 20) {
                s32 index = object->field_6A;
                DuelCardRecord *base = gDuel_aCardRecords;
                gDuel_wSceneStateFlags = 0x4008;
                card = &base[index];
                card->flags &= ~0x1000;
                if (gDuel_bPlacementNeedsCardMoveSetup)
                gDuel_wSceneStateFlags = 8;
            }
            break;
        }
    }
}
