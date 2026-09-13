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
#include "display_object_api.h"
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

#define H(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define B(p, o) (*((u8 *)(p) + (o)))
#define W(p, o) (*(u32 *)((u8 *)(p) + (o)))

void DuelEffect_ApplyRitual(void)
{
    DisplayObject *object;
    DuelCardRecord *card;
    u8 *text;
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
            H(D_8009B17C, 0x1A) = gDuel_wEffectCardID;
            func_8003FF88(0x8022);
        } else {
            goto done;
        }
        return;
    }
    switch (D_8009B210 & 0xF) {
    case 0:
        if (B(D_8009B17C, 0x1D)) {
            D_8009B19C = B(D_800E9EF0.slots[3], 0x6A);
            func_80024914(&D_801A7AD8[B(D_800E9EF0.slots[2], 0x6A)]);
            func_80024914(&D_801A7AD8[B(D_800E9EF0.slots[3], 0x6A)]);
            func_80024914(&D_801A7AD8[B(D_800E9EF0.slots[4], 0x6A)]);
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
            H(object, 0x30) = 90;
            S(object, 0x32) = -34;
            H(object, 0x60) = 24;
            attribute = W(object, 4);
            object_flags = H(object, 8);
            H(object, 0x46) = 0;
            H(object, 0x44) = 0;
            W(object, 4) = attribute & 0xF7FFFFFF;
            H(object, 8) = object_flags | 4;
            DisplayObject_ResetVelocity((DisplayObjectVelocity *)object);
            H(object, 0x38) = 597;
            D_8009B1C0 = object;
            SD_SEPlayFull(0x26);
        }
        DisplayObject_StepPositionY((DisplayObjectVelocity *)object);
        scale = H(object, 0x44) + 170;
        countdown = H(object, 0x60) - 1;
        H(object, 0x44) = scale;
        H(object, 0x46) = scale;
        H(object, 0x60) = countdown;
        if ((s32)((u32)countdown << 16) <= 0) {
            s32 attribute;
            s16 object_flags;
            H(object, 0x46) = 4096;
            H(object, 0x44) = 4096;
            H(object, 0x32) = 22;
            D_8009B210 = 2;
            attribute = W(object, 4) | 0x08000000;
            object_flags = H(object, 8) & 0xFFFB;
            W(object, 4) = attribute;
            H(object, 8) = object_flags;
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
                W(D_800E9EF0.slots[0], 4) |= 0x50000000;
                W(D_800E9EF0.slots[0], 4) &= ~0x08000000;
                D_800E9EF0.slots[1] = func_80019564((DisplayObjectConfigView *)object);
                func_800428EC((u8 *)D_800E9EF0.slots[1], -1);
                W(D_800E9EF0.slots[1], 4) |= 0x60000000;
                W(D_800E9EF0.slots[1], 4) &= ~0x08000000;
                func_80029528(1);
            }
        } else {
            DisplayObject *first = D_800E9EF0.slots[0];
            DisplayObject *second = D_800E9EF0.slots[1];
            value = S(first, 0x44) + 128;
            H(second, 0x46) = value;
            H(second, 0x44) = value;
            H(first, 0x46) = value;
            H(first, 0x44) = value;
            value = B(D_800E9EF0.slots[0], 0xC);
            value -= 4;
            if (value < 0)
                value = 0;
            value = value | ((value << 16) | (value << 8));
            W(D_800E9EF0.slots[0], 0xC) = value;
            W(D_800E9EF0.slots[1], 0xC) = value;
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
            /* The raw word view of the data pointer preserves retail scheduling. */
            H(((u8 *)W(card, 4)), 0) = ritual = D_8009B1A0;
            data = ((u8 *)W(card, 4));
            D_8009B210 = state | 0x80;
            card->card_id = ritual;
            value = B(data, 3);
            y = rects[21];
            rects[4] = rects[20] + 56;
            rects[6] = 8;
            rects[7] = 88;
            rects[5] = y;
            StoreImage((RECT *)&rects[4],
                (u32 *)(D_8018C2D8 + 1408 * value));
            break;
        }
        func_80024D34(D_8009B19C, *((s8 *)((u8 *)W(card, 4)) + 2));
        object = card->object;
        D_800E9EF0.slots[0] = object;
        S(object, 0x32) = -240;
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
            H(object, 0x28) = 134;
            H(object, 0x2A) = 42;
            H(object, 0x2C) = 16;
            B(object, 0x6C) = 1;
            object->update = func_8001EC70;
        } else if (!(D_8009B210 & 0x40)) {
            if (!func_80042B40(1)) {
                gDuel_wSelectedCardID =
                    D_8015C424_cards.field_cards[B(object, 0x6A)].card_id;
                text = TextBox_CreateFlagged(0, 33, 72, 110, 176, 48, 32);
                B(text, 0x5A) = 8;
                B(text, 0x5B) = 16;
                do {
                    func_80039794();
                } while (!W(text, 0x30));
                D_8009B210 |= 0x40;
            }
        } else if (!(D_8009B210 & 0x20)) {
            if (!Dialog_ReadChoiceInput((u8 *)D_800EB0F8) &&
                (gInput_wPad1Pressed & 0xC0)) {
                SD_SEPlayFull(7);
                H(object, 0x28) = 134;
                S(object, 0x2A) = -128;
                H(object, 0x2C) = 16;
                B(object, 0x6C) = 1;
                object->update = func_8001EC70;
                card = &D_801A7AD8[B(object, 0x6A)];
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
            S(object, 0x60) = -1024;
        }
        Widget_SlideSine((DisplayObjectPosition *)object,
            S(object, 0x30), -24, S(object, 0x60));
        phase = H(object, 0x60) + 42;
        H(object, 0x60) = phase;
        if (phase >= 0) {
            S(object, 0x32) = -24;
done:
            gDuel_wCardEffectFlags = 0;
        }
        break;
    }
    }
}
