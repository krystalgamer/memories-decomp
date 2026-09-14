#include "../types.h"
#define GDIALOG_CHOICE_IN_DATA
#define GINPUT_PAD1_PRESSED_IN_DATA
#define GDUEL_WSELECTEDCARDID_IN_DATA
#define D_8009B360_AS_SIDE_ARRAY
#define DUEL_CARD_STAGING_DECK_VIEW
#define DISPLAY_OBJECT_WORK_RITUAL_VIEW
#define DISPLAY_OBJECT_MOTION_BYTE_CALLBACK
#include "duel_action_lock.h"
#include "duel_check_ritual.h"
#include "duel_ritual_controller.h"
#include "duel_card_record_lifecycle.h"
#include "duel_card_display_state.h"
#include "duel_card_staging.h"
#include "duel_effect_request.h"
#include "duel_side_state.h"
#include "display_object_work_slots.h"
#include "display_object_helpers.h"
#include "display_object_interpolation.h"
#include "display_object_motion.h"
#include "display_object_core.h"
#include "func_8001944C.h"
#include "func_80019564.h"
#include "func_800291E0.h"
#include "duel_effect_allocate_request.h"
#include "duel_effect.h"
#include "text_box_lifecycle.h"
#include "dialog_read_choice_input.h"
#include "dialog_choice.h"
#include "input.h"
#include "sound.h"
#include "func_80039794.h"
#include "../psyq/rand.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

void DuelEffect_ApplyRitual(void)
{
    DisplayObject *object;
    DuelCardRecord *card;
    DuelEffectChannel *text;
    s32 value;
    s32 timer;
    u16 flags;

    if (!DuelEffect_MarkInitialized()) {
        D_8009B1A0 = Duel_CheckRitual(
            &D_800E9EF0.ritual.result, gDuel_wEffectCardID);
        if (D_8009B1A0) {
            func_80019CC8((void *)(s32)D_8009B1A0);
            D_8009B17C = DuelEffect_AllocateRequest(22);
            D_8009B210 = 0;
            ((DuelEffectRequest *)D_8009B17C)->field_1A = gDuel_wEffectCardID;
            func_8003FF88(0x8022);
        } else {
            goto done;
        }
        return;
    }
    switch (D_8009B210 & 0xF) {
    case 0:
        if (((DuelEffectRequest *)D_8009B17C)->field_1D) {
            D_8009B19C = D_800E9EF0.slots[3]->field_6A;
            func_80024914(&D_801A7AD8[D_800E9EF0.slots[2]->field_6A]);
            func_80024914(&D_801A7AD8[D_800E9EF0.slots[3]->field_6A]);
            func_80024914(&D_801A7AD8[D_800E9EF0.slots[4]->field_6A]);
            D_8009B210 = 1;
        }
        break;
    case 1: {
        u16 scale;
        s32 countdown;
        object = D_8009B1C0;
        if (!(D_8009B210 & 0x80)) {
            s32 attribute;
            s16 object_flags;
            D_8009B210 |= 0x80;
            object = (DisplayObject *)func_800291E0(1, -1, -1);
            object->field_30.h.field_30 = 90;
            *(s16 *)&object->field_30.h.field_32 = -34;
            object->field_60 = 24;
            attribute = object->attribute;
            object_flags = object->flags;
            object->field_44.h.field_46 = 0;
            object->field_44.h.field_44 = 0;
            object->attribute = attribute & 0xF7FFFFFF;
            object->flags = object_flags | 4;
            DisplayObject_ResetVelocity((DisplayObjectVelocity *)object);
            object->field_38.h.field_38 = 597;
            D_8009B1C0 = object;
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
            object->field_30.h.field_32 = 22;
            D_8009B210 = 2;
            attribute = object->attribute | 0x08000000;
            object_flags = object->flags & 0xFFFB;
            object->attribute = attribute;
            object->flags = object_flags;
        }
        break;
    }
    case 2:
        object = D_8009B1C0;
        if (!(D_8009B210 & 0x80)) {
            D_8009B210 |= 0x80;
            D_8009B20C[1] = 32;
        }
        if (!(D_8009B210 & 0x40)) {
            timer = *(u16 *)&D_8009B20C[1] - 1;
            D_8009B20C[1] = timer;
            if ((s32)((u32)timer << 16) <= 0) {
                D_8009B210 |= 0x40;
                func_8001944C(object);
                D_800E9EF0.slots[0] = func_80019564((DisplayObjectConfigView *)object);
                D_800E9EF0.slots[0]->attribute |= 0x50000000;
                D_800E9EF0.slots[0]->attribute &= ~0x08000000;
                D_800E9EF0.slots[1] = func_80019564((DisplayObjectConfigView *)object);
                func_800428EC((u8 *)D_800E9EF0.slots[1], -1);
                D_800E9EF0.slots[1]->attribute |= 0x60000000;
                D_800E9EF0.slots[1]->attribute &= ~0x08000000;
                func_80029528(1);
            }
        } else {
            DisplayObject *first = D_800E9EF0.slots[0];
            DisplayObject *second = D_800E9EF0.slots[1];
            value = first->field_44.h.field_44 + 128;
            second->field_44.h.field_46 = value;
            second->field_44.h.field_44 = value;
            first->field_44.h.field_46 = value;
            first->field_44.h.field_44 = value;
            value = *(u8 *)&D_800E9EF0.slots[0]->field_0C;
            value -= 4;
            if (value < 0)
                value = 0;
            value = value | ((value << 16) | (value << 8));
            D_800E9EF0.slots[0]->field_0C = value;
            D_800E9EF0.slots[1]->field_0C = value;
            if (!value) {
                func_8004036C(D_800E9EF0.slots[0]);
                func_8004036C(D_800E9EF0.slots[1]);
                D_8009B210 = 3;
            }
        }
        break;
    case 3: {
        u16 state = D_8009B210;
        u16 *rects = D_800EA128;
        u8 *data;
        card = &D_801A7AD8[D_8009B19C];
        if (!(state & 0x80)) {
            u16 ritual;
            u16 y;
            /* The data pointer is reloaded through its address as u8 **, a
               non-struct read; as a plain card->data read the function's
               schedule changes (17 differences). */
            *(u16 *)card->data = ritual = D_8009B1A0;
            data = *(u8 **)&card->data;
            D_8009B210 = state | 0x80;
            card->card_id = ritual;
            value = data[3];
            y = rects[21];
            rects[4] = rects[20] + 56;
            rects[6] = 8;
            rects[7] = 88;
            rects[5] = y;
            StoreImage((RECT *)&rects[4],
                (u32 *)(D_8018C2D8 + 1408 * value));
            break;
        }
        func_80024D34(D_8009B19C, ((s8 *)card->data)[2]);
        object = card->object;
        D_800E9EF0.slots[0] = object;
        *(s16 *)&object->field_30.h.field_32 = -240;
        if (D_8009B360[D_8009B1D5] >= 0) {
            card->flags &= ~0x200;
            if (rand() & 1)
                card->flags |= 0x200;
            goto state_five;
        }
        D_800E9EF0.slots[1] = (DisplayObject *)func_80017F04(card, 134, 240);
        D_8009B210 = 4;
    }
    case 4:
        object = D_800E9EF0.slots[1];
        if (!(D_8009B210 & 0x80)) {
            D_8009B210 |= 0x80;
            object->position.h.field_28 = 134;
            object->position.h.field_2A = 42;
            object->field_2C.h.field_2C = 16;
            object->field_6C = 1;
            object->update = func_8001EC70;
        } else if (!(D_8009B210 & 0x40)) {
            if (!func_80042B40(1)) {
                gDuel_wSelectedCardID =
                    D_8015C424_cards.field_cards[object->field_6A].card_id;
                text = TextBox_CreateFlagged(0, 33, 72, 110, 176, 48, 32);
                text->field_5A = 8;
                text->field_5B = 16;
                do {
                    func_80039794();
                } while (!text->field_30);
                D_8009B210 |= 0x40;
            }
        } else if (!(D_8009B210 & 0x20)) {
            if (!Dialog_ReadChoiceInput((u8 *)D_800EB0F8) &&
                (gInput_wPad1Pressed & 0xC0)) {
                SD_SEPlayFull(7);
                object->position.h.field_28 = 134;
                *(s16 *)&object->position.h.field_2A = -128;
                object->field_2C.h.field_2C = 16;
                object->field_6C = 1;
                object->update = func_8001EC70;
                card = &D_801A7AD8[object->field_6A];
                flags = card->flags & ~0x200;
                card->flags = flags;
                if (gDialog_bChoice)
                    card->flags = flags | 0x200;
                TextBox_Destroy(D_800EB0F8);
                D_8009B210 |= 0x20;
            }
        } else if (!func_80042B40(1)) {
            func_8004036C(object);
state_five:
            D_8009B210 = 5;
        }
        break;
    case 5: {
        s16 phase;
        object = D_800E9EF0.slots[0];
        if (!(D_8009B210 & 0x80)) {
            D_8009B210 |= 0x80;
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
