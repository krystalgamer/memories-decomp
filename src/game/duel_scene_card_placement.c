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
#include "func_80019BA0.h"
#include "func_800291E0.h"
#include "text_box_lifecycle.h"
#include "duel_effect.h"
#include "file_transfer.h"
#include "sound.h"
#include "../unmatched.h"
#include "duel_scene_card_placement.h"

#define PLACEMENT_PX(object) ((object)->field_30.h.field_30)
#define PLACEMENT_PY(object) ((object)->field_30.h.field_32)
#define PLACEMENT_TX(object) ((object)->position.h.field_28)
#define PLACEMENT_TY(object) ((object)->position.h.field_2A)
#define PLACEMENT_VX(object) ((object)->field_34.h.field_36)
#define PLACEMENT_VY(object) ((object)->field_38.h.field_38)
#define PLACEMENT_ALPHA(object) ((object)->field_20.b.field_21)
#define PLACEMENT_ROTATION(object) ((object)->field_20.b.field_22)
#define PLACEMENT_REPLAY_CARD(object) (D_8015C424_cards.field_cards[(object)->field_6A])

s16 D_8009B206;

static __inline__ s32 placement_velocity(s32 distance)
{
    return distance / 8;
}

void DuelScene_UpdateCardPlacement(void)
{
    DuelFieldDisplaySource *side = (DuelFieldDisplaySource *)
        (D_800E9F10 + D_8009B1D5 * DUEL_SELECTION_SIDE_SIZE);
    DisplayObject *object;
    DuelCardRecord *card;
    DuelEffectRequest *effect;
    s32 n;
    s32 value;
    s32 phase_clear;
    s32 x;

    if (!(gDuel_wSceneStateFlags & 0x8000)) {
        DisplayObject **slots;
        DisplayObject **cursor;
        DuelHandSlot *hand;
        s16 y;
        x = 260;
        value = 4;
        gDuel_wSceneStateFlags |= 0x8000;
        D_8009B1B4 = (DuelCardPickCursor *)side;
        D_8009B218 = 0;
        D_8009B1B9 = 0;
        D_8009B154 = 0;
        n = 5;
        y = 82;
        slots = D_800E9EF0;
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
        object = D_800E9EF0[0];
        PLACEMENT_TX(object) = 64;
        PLACEMENT_TY(object) = 82;
        DisplayObject_SetDepthOffset(object, value);
        if ((u8)(object->field_6A % 15) < 5)
        D_8009B218 = 1;
        slots = D_800E9EF0;
        if (!slots[1]) {
            if (slots[0]->field_68 < 20)
            D_8009B218 = 1;
            D_8009B150 = PLACEMENT_REPLAY_CARD(object).card_id;
            PLACEMENT_TX(object) = 134;
            PLACEMENT_TY(object) = 42;
            gDuel_wSceneStateFlags |= 0x4000;
        } else {
            D_8009B1C8->rank.field_07++;
        }
        n = 0;
        do {
            hand = &D_800EA030[n];
            if (hand->active_09) {
                u8 *child = hand->child;
                hand->object = 0;
                DisplayObject_ReleaseIfPresent(child);
                hand->child = 0;
            }
            n++;
        } while (n < 5);
        DisplayObject_ReleaseIfPresent(side->object);
        D_8009B162 = 8;
        side->object = 0;
        D_8009B174 = 1;
        return;
    }

    switch (D_8009B174 & 15) {
    case 1:
        {
            DisplayObject **slots;
            {
                DisplayObject **cursor;
                DuelCardRecord *cards;
                DisplayObjectCallback callback;
                s32 active;
                if (!(D_8009B174 & 0x80)) {
                    D_8009B174 |= 0x80;
                    if (gDuel_wSceneStateFlags & 0x4000)
                    return;
                    n = 5;
                    cards = D_801A7AD8;
                    active = 1;
                    callback = (DisplayObjectCallback)func_80019BD0;
                    slots = D_800E9EF0;
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
                D_8009B174 = 2;
            }
        }
    case 2:
        {
            DisplayObject **slots;
            {
                DisplayObject **cursor;
                if (!(D_8009B174 & 0x80)) {
                    DisplayObject **cursor;
                    s32 active;
                    D_8009B174 |= 0x80;
                    n = 5;
                    active = 1;
                    slots = D_800E9EF0;
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
                slots = D_800E9EF0;
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
                if (!(D_8009B162 | value)) {
                    D_8009B174 = 3;
                    if (gDuel_wSceneStateFlags & 0x4000)
                    gDuel_wSceneStateFlags = 8;
                }
            }
            break;
        }
    case 3:
        {
            DisplayObject **slots;
            if (!(D_8009B174 & 0x80)) {
                D_8009B174 |= 0x80;
                D_8009B1B9++;
                if ((s8)D_8009B1B9 >= 6) {
                    D_8009B174 = 8;
                    return;
                }
                {
                    DisplayObject *selected = D_800E9EF0[(s8)D_8009B1B9];
                    if (!selected) {
                        D_8009B174 = 8;
                        return;
                    }
                    object = selected;
                }
                D_800E9EF0[1] = object;
                DisplayObject_ResetVelocity((DisplayObjectVelocity *)object);
                object->field_60 = 8;
                PLACEMENT_VX(object) = placement_velocity((64 - (s16)PLACEMENT_PX(object)) * 256);
                PLACEMENT_VY(object) = placement_velocity((82 - (s16)PLACEMENT_PY(object)) * 256);
                DisplayObject_SetDepthOffset(D_800E9EF0[0], (s8)((u8)object->field_16 - 1));
                SD_SEPlayFull(10);
                D_8009B150 = 0;
                if (D_800E9EF0[0]) {
                    s32 result = Duel_CheckFusion(PLACEMENT_REPLAY_CARD(D_800E9EF0[0]).card_id,
                    PLACEMENT_REPLAY_CARD(object).card_id);
                    D_8009B150 = result;
                    if ((s16)result) {
                        D_8009B150 = result | 0x8000;
                    } else {
                        s32 equip_result = Duel_CheckEquip(PLACEMENT_REPLAY_CARD(object).card_id,
                        PLACEMENT_REPLAY_CARD(D_800E9EF0[0]).card_id);
                        D_8009B150 = equip_result;
                        if ((s16)equip_result)
                        goto request_combination;
                        D_8009B150 = Duel_CheckEquip(PLACEMENT_REPLAY_CARD(D_800E9EF0[0]).card_id,
                        PLACEMENT_REPLAY_CARD(object).card_id);
                    }
                    if (!((DuelPlacementCardIdCell *)&D_8009B150)->value)
                    return;
request_combination:
                    func_80029164(0, D_8009B150 & 0xFFF);
                }
                return;
            }
            slots = D_800E9EF0;
            object = slots[1];
            DisplayObject_StepPositionX((DisplayObjectVelocity *)object);
            object->field_60--;
            if (object->field_60)
            return;
            D_8009B210 = 0;
            if (!slots[0]) {
                slots[0] = object;
                D_8009B174 = 3;
                return;
            }
            object->field_30.word = slots[0]->field_30.word;
            if (!((DuelPlacementCardIdCell *)&D_8009B150)->value) {
                D_8009B174 = 7;
                D_8009B218 = -1;
            } else if (((DuelPlacementCardIdCell *)&D_8009B150)->value & 0x8000) {
                D_8009B218 = 1;
                D_8009B154 = 0;
                D_8009B174 = 6;
                D_8009B1C8->rank.fusions_initiated++;
            } else {
                D_8009B174 = 5;
                D_8009B154 += 500;
                D_8009B1C8->rank.equips_used++;
            }
            break;
        }
    case 4:
        {
            DisplayObject **slots;
            if ((D_8009B0F4_abs & 0x2000030) | D_8009B134_abs)
            return;
            if (!(D_8009B174 & 0x80)) {
                D_8009B174 |= 0x80;
                D_8009B210 = 0;
                return;
            }
            switch (D_8009B210 & 15) {
            case 0:
                {
                    DisplayObject **slots;
                    {
                        s32 card_index = D_800E9EF0[0]->field_6A;
                        DuelEffectResourceRecord *resource = D_800EA0E8;
                        card = &D_801A7AD8[card_index];
                        slots = D_800E9EF0;
                        if (!(D_8009B210 & 0x80)) {
                            D_8009B210 |= 0x80;
                            if ((s16)D_8009B150 & 0x8000) {
                                ((DuelDeckCardRecord *)card->data)->id = D_8009B150 & 0xFFF;
                                D_8009B210 |= 0x40;
                                card->card_id = D_8009B150 & 0xFFF;
                                value = ((DuelDeckCardRecord *)card->data)->data_block_index;
                                resource->rects[0].x = resource->src_x + 56;
                                resource->rects[0].y = resource->src_y;
                                resource->rects[0].w = 8;
                                resource->rects[0].h = 88;
                                StoreImage2(&resource->rects[0], (u32 *)(D_8018C2D8 + value * 1408));
                            }
                            return;
                        }
                        if (D_8009B210 & 0x40) {
                            Duel_SetupCardRecord(card_index, (s8)((DuelDeckCardRecord *)card->data)->index_02);
                            DuelCard_RemoveFromField(card);
                        }
                        value = Duel_CalcCardStats(card);
                        object = (DisplayObject *)func_800291E0(0, value & 0xFFFF, value >> 16);
                        PLACEMENT_PY(object) = 16;
                        PLACEMENT_PX(object) = PLACEMENT_PX(D_800E9EF0[0]) - 44;
                        object->flags &= ~0x40;
                        D_8009B1C0 = object;
                        func_80019BA0(D_800E9EF0[0], 0, 64, 8);
                        if (slots[1]) {
                            s32 second = (s16)PLACEMENT_PX(slots[1]);
                            s32 first = (s16)PLACEMENT_PX(D_800E9EF0[0]);
                            second += 52;
                            PLACEMENT_PX(D_8009B1C0) = (first + second) / 2 - 70;
                            func_80019BA0(slots[1], 0, 64, 8);
                        }
                        D_8009B210 = 1;
                    }
                    break;
                }
            case 1:
                {
                    DisplayObject **slots;
                    s32 alpha;
                    if (DisplayObject_FindAllocatedByTag(1))
                    return;
                    if (!(D_8009B210 & 0x80)) {
                        slots = D_800E9EF0;
                        slots[0]->flags &= ~0x40;
                        D_8009B210 |= 0x80;
                        if (slots[1])
                        slots[1]->flags &= ~0x40;
                        PLACEMENT_ALPHA(D_8009B1C0) = 192;
                        D_8009B1C0->flags |= 0x44;
                        return;
                    }
                    alpha = PLACEMENT_ALPHA(D_8009B1C0) + 8;
                    PLACEMENT_ALPHA(D_8009B1C0) = alpha;
                    if ((s8)alpha >= 0) {
                        DisplayObject *entry = D_8009B1C0;
                        u16 flags = entry->flags;
                        PLACEMENT_ALPHA(entry) = 0;
                        D_8009B210 = 2;
                        entry->flags = flags & ~4;
                    }
                    break;
                }
            case 2:
                {
                    DisplayObject **slots;
                    card = &D_801A7AD8[D_800E9EF0[0]->field_6A];
                    object = D_8009B1C0;
                    if (!(D_8009B210 & 0x80)) {
                        D_8009B210 |= 0x80;
                        object->field_60 = 30;
                        if (!((s16)D_8009B150 & 0x8000)) {
                            PLACEMENT_TX(object) = 0;
                            PLACEMENT_TY(object) = 500;
                            D_8009B210 |= 0x40;
                            object->field_2C.h.field_2C = card->stat_modifier;
                            if (D_8009B206 == 0x291) {
                                PLACEMENT_TY(object) = 1000;
                                D_8009B154 += 500;
                            }
                        }
                    }
                    if (D_8009B210 & 0x40) {
                        s32 below;
                        u16 limit;
                        PLACEMENT_TX(object) += 31;
                        below = (s16)PLACEMENT_TX(object) < (s16)PLACEMENT_TY(object);
                        limit = PLACEMENT_TY(object);
                        if (!below) {
                            D_8009B210 &= ~0x40;
                            PLACEMENT_TX(object) = limit;
                            object->field_60 = 16;
                        }
                        card->stat_modifier = object->field_2C.h.field_2C + PLACEMENT_TX(object);
                        value = Duel_CalcCardStats(card);
                        D_800EA0E8[0].field_32 = value;
                        D_800EA0E8[0].field_34 = value >> 16;
                        return;
                    }
                    object->field_60--;
                    if (object->field_60 > 0)
                    return;
                    {
                        DisplayObject *entry = D_8009B1C0;
                        *(u16 *)((u32)entry + (u32)&((DisplayObject *)0)->flags) = entry->flags | 4;
                    }
                    slots = D_800E9EF0;
                    DisplayObject_ReleaseIfPresent(slots[0]);
                    DisplayObject_ReleaseIfPresent(slots[1]);
                    slots[1] = 0;
                    slots[0] = 0;
                    object = (DisplayObject *)func_80017F04(card, 64, 82);
                    D_800E9EF0[0] = object;
                    object->field_20.word = 0;
                    D_8009B210 = 3;
                    object->flags &= ~0x44;
                    break;
                }
            case 3:
                {
                    DisplayObject **slots;
                    s32 alpha;
                    object = D_800E9EF0[0];
                    if (!(D_8009B210 & 0x80)) {
                        alpha = PLACEMENT_ALPHA(D_8009B1C0) + 8;
                        PLACEMENT_ALPHA(D_8009B1C0) = alpha;
                        if ((u8)alpha < 64)
                        return;
                        {
                            u16 position = PLACEMENT_PX(D_8009B1C0);
                            D_8009B210 |= 0x80;
                            PLACEMENT_PX(object) = position + 44;
                        }
                        func_80019BA0(object, 192, 0, 8);
                        object->flags |= 0x40;
                        func_80029528(0);
                        return;
                    }
                    if (!DisplayObject_FindAllocatedByTag(1))
                    D_8009B210 = 4;
                    break;
                }
            case 4:
                {
                    DisplayObject **slots;
                    object = D_800E9EF0[0];
                    if (!(D_8009B210 & 0x80)) {
                        D_8009B210 |= 0x80;
                        DisplayObject_SavePosition((DisplayObjectSnapshot *)object);
                        object->field_60 = 0;
                    }
                    DisplayObject_InterpolatePositionCosine((DisplayObjectPosition *)object, 64, 82, object->field_60);
                    object->field_60 += 128;
                    if (object->field_60 >= 2048)
                    D_8009B174 = 3;
                    break;
                }
            }
            break;
        }
    case 5:
        {
            DisplayObject **slots;
            if (!(D_8009B174 & 0x80)) {
                DisplayObject **slots;
                slots = D_800E9EF0;
                slots[0]->flags &= ~0x40;
                D_8009B174 |= 0xC0;
                func_80019BA0(slots[1], 0, 64, 8);
                object = D_800E9EF0[0];
                if (object->field_68 != 23)
                object = slots[1];
                D_8009B206 = PLACEMENT_REPLAY_CARD(object).card_id;
                return;
            }
            if (D_8009B174 & 0x40) {
                DisplayObject **slots;
                if (DisplayObject_FindAllocatedByTag(1))
                return;
                {
                    s32 phase = D_8009B174;
                    if (phase & 0x20) {
                        phase_clear = phase & 0x9F;
                        goto finish_effect_step;
                    }
                }
                {
                    DisplayObject **release_slots = D_800E9EF0;
                    D_8009B174 |= 0x20;
                    if (release_slots[1]->field_68 < 20) {
                        DisplayObject_ReleaseIfPresent(D_800E9EF0[0]);
                        D_800E9EF0[0] = release_slots[1];
                    } else {
                        DisplayObject_ReleaseIfPresent(release_slots[1]);
                    }
                }
                {
                    DisplayObject **publish_slots = D_800E9EF0;
                    publish_slots[1] = 0;
                    D_800E9EF0[0]->flags |= 0x40;
                    func_80019BA0(D_800E9EF0[0], 192, 0, 8);
                }
                return;
            }
            if (!(D_8009B174 & 0x20)) {
                DisplayObject **slots;
                D_8009B174 |= 0x20;
                effect = (DuelEffectRequest *)DuelEffect_AllocateRequest(((u32)((u16)D_8009B206 - 301) < 2) * 4);
                effect->field_00 = PLACEMENT_PX(D_800E9EF0[0]) + 26;
                D_8009B17C = (u8 *)effect;
                effect->field_02 = PLACEMENT_PY(D_800E9EF0[0]) + 30;
                if (D_8009B206 == 302)
                effect->field_1A = 1;
                if (D_8009B206 == 0x291)
                effect->field_1A = 10;
                SD_SEPlayFull(22);
            } else if (((DuelEffectRequest *)D_8009B17C)->field_1D) {
                D_8009B174 = 4;
            }
            break;
        }
    case 6:
        {
            DisplayObject **slots;
            if (!(D_8009B174 & 0x80)) {
                DisplayObject **slots;
                D_8009B174 |= 0xC0;
                func_80019CC8((void *)(u32)(D_8009B150 & 0xFFF));
                slots = D_800E9EF0;
                slots[0]->flags &= ~0x40;
                func_80019BA0(slots[1], 0, 64, 8);
            }
            if (D_8009B174 & 0x40) {
                DisplayObject **slots;
                if (DisplayObject_FindAllocatedByTag(1))
                return;
                {
                    s32 phase = D_8009B174;
                    if (phase & 0x20) {
                        phase_clear = phase & 0x9F;
                        goto finish_effect_step;
                    }
                }
                slots = D_800E9EF0;
                D_8009B174 |= 0x20;
                func_80019BA0(slots[1], 192, 0, 8);
                PLACEMENT_PX(slots[1]) = 140;
                D_800E9EF0[0]->flags |= 0x40;
                func_80019BA0(D_800E9EF0[0], 192, 0, 8);
                return;
finish_effect_step:
                D_8009B174 = phase_clear;
                return;
            }
            if (!(D_8009B174 & 0x20)) {
                DisplayObject **slots;
                D_800E9EF0[0]->field_60 = 0;
                PLACEMENT_TX(D_800E9EF0[0]) = 1;
                PLACEMENT_TY(D_800E9EF0[0]) = 0x2680;
                D_8009B174 |= 0x20;
                D_800E9EF0[0]->field_2C.h.field_2E = 1;
                {
                    DuelEffectRequest *request;
                    request = (DuelEffectRequest *)DuelEffect_AllocateRequest(1);
                    request->field_00 = 128;
                    (*(s16 *)((u32)request + (u32)&((DuelEffectRequest *)0)->field_02)) = PLACEMENT_PY(D_800E9EF0[0]) + 30;
                    D_8009B17C = (u8 *)request;
                    slots = D_800E9EF0;
                    if (slots[0]->field_68 >= 20 || slots[1]->field_68 >= 20)
                    request->field_1A = 1;
                }
                SD_SEPlayFull(24);
                return;
            }
            if (((DuelEffectRequest *)D_8009B17C)->field_1D) {
                DisplayObject **slots;
                slots = D_800E9EF0;
                slots[0]->flags &= ~0x40;
                slots[1]->flags &= ~0x40;
                SD_SEPlayFull(25);
                D_8009B174 = 4;
            } else {
                object = D_800E9EF0[0];
                object->field_2C.h.field_2E += 8;
                object->field_60 += D_800E9EF0[0]->field_2C.h.field_2E;
                PLACEMENT_TY(object) -= object->field_60 / 12;
                n = (s16)PLACEMENT_TY(object) >> 8;
                x = rcos(object->field_60) * n / 4096;
                value = rsin(object->field_60) * n / 4096;
                PLACEMENT_PX(object) = 102 - x;
                PLACEMENT_PY(object) = 82 - value;
                slots = D_800E9EF0;
                PLACEMENT_PX(slots[1]) = x + 102;
                PLACEMENT_PY(slots[1]) = value + 82;
            }
            break;
        }
    case 7:
        {
            DisplayObject **slots;
            slots = D_800E9EF0;
            object = slots[0];
            if (!(D_8009B174 & 0x80)) {
                D_8009B174 |= 0x80;
                object->flags |= 4;
                object->attribute &= ~0x08000000;
                SD_SEPlayFull(26);
                if (slots[1]->field_68 >= 20 && object->field_68 < 20) {
                    DisplayObject *other = slots[1];
                    slots[1] = object;
                    object = other;
                    slots[0] = other;
                }
                DisplayObject_ResetVelocity((DisplayObjectVelocity *)object);
                PLACEMENT_VX(object) = -((rand() & 255) + 768);
                PLACEMENT_VY(object) = -640;
            }
            PLACEMENT_VX(object) = DisplayObject_StepTowardZero(PLACEMENT_VX(object), 8);
            PLACEMENT_VY(object) = DisplayObject_StepToward(PLACEMENT_VY(object), 2048, 96);
            DisplayObject_StepPositionXY((DisplayObjectVelocity *)object);
            if ((s16)PLACEMENT_PX(object) < -52 || (s16)PLACEMENT_PY(object) >= 240) {
                DisplayObject_ReleaseIfPresent(object);
                D_800E9EF0[0] = D_800E9EF0[1];
                TextBox_Destroy(D_800EB0F8);
                D_8009B174 = 3;
            }
            break;
        }
    case 8:
        {
            DisplayObject *object;
            DuelCardRecord *card;
            DisplayObject **slots;
            object = D_800E9EF0[0];
            gDuel_wSceneStateFlags = 6;
            if (object->field_68 < 20) {
                s32 index = object->field_6A;
                DuelCardRecord *base = D_801A7AD8;
                gDuel_wSceneStateFlags = 0x4008;
                card = &base[index];
                card->flags &= ~0x1000;
                if (D_8009B218)
                gDuel_wSceneStateFlags = 8;
            }
            break;
        }
    }
}
