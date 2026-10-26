#define DUEL_CARD_STAGING_DECK_VIEW
#include "../types.h"
#include "duel_scene_state.h"
#define D_8009B360_AS_SIDE_ARRAY
#define D_800EAE8F_IS_ARRAY
#define GDIALOG_CHOICE_IN_DATA
#define GDUEL_WSELECTEDCARDID_IN_DATA
#define GINPUT_PAD1_PRESSED_IN_DATA
#include "ai.h"
#include "display_object_work_slots.h"
#include "display_object_interpolation.h"
#include "display_object_core.h"
#include "duel_card_display_objects.h"
#include "duel_card.h"
#include "duel_card_staging.h"
#include "duel_card_record_lifecycle.h"
#include "duel_side_state.h"
#include "ai_controller_handoff.h"
#include "duel_scene_callbacks.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "duel_effect_request.h"
#include "duel_action_lock.h"
#include "duel_trap_resolution.h"
#include "duel_screen_tables.h"
#include "dialog_choice.h"
#include "dialog_read_choice_input.h"
#include "text_box_lifecycle.h"
#include "text_box_update_channels.h"
#include "duel_select_trap_by_card_id.h"
#include "duel_effect_set_entry_range_layout.h"

/* One screen height above the top: -0xF0 in the US build, -0x100 in the
   European one, whose wrapper defines its own. */
#ifndef DUEL_CARD_OFFSCREEN_Y
#define DUEL_CARD_OFFSCREEN_Y -0xF0
#endif
#include "input.h"
#include "sound.h"
#include "../unmatched.h"

#ifndef DUEL_CARD_STAGING_CONFIRM_MASK
#define DUEL_CARD_STAGING_CONFIRM_MASK PAD_BUTTON_CONFIRM_MASK
#endif

#define DISPLAY_OBJECT_POSITION_VIEW(object) \
    ((DisplayObjectPosition *)(object))
#define DISPLAY_OBJECT_SNAPSHOT_VIEW(object) \
    ((DisplayObjectSnapshot *)(object))

void DuelScene_UpdateCardMove(void)
{
    DisplayObject *object;
    DuelEffectChannel *box;
    DuelCardRecord *card;
    DuelCardRecord *scratch;
    DuelEffectRequest *request;
    u16 flags;
    u16 z;
    s32 slot;

    object = gDisplayObject_apWorkSlots[0];
    if (!(gDuel_wSceneStateFlags & DUEL_SCENE_FLAG_INITIALIZED)) {
        gDuel_wSceneStateFlags |= DUEL_SCENE_FLAG_INITIALIZED;
        if (gDuel_wSceneStateFlags & 0x4000) {
            goto state_four;
        }
        if (gDuel_bSide0ControllerSelector[gDuel_bActiveSide] >= 0 &&
            object->field_68 < 0x14) {
            gDuel_aCardRecords[object->field_6A].flags &= ~0x200;
            if (gAi_bGuardianStarChoice[0] & 1) {
                gDuel_aCardRecords[object->field_6A].flags |= 0x200;
            }
            goto state_four;
        }
        gDuel_bSceneSubStep = 1;
        DisplayObject_SavePosition(DISPLAY_OBJECT_SNAPSHOT_VIEW(object));
        object->field_60 = 0;
        if ((s16)object->field_30.h.field_30 == 0x86 ||
            (s16)object->field_30.h.field_32 == 0x2A) {
            gDuel_bSceneSubStep = 2;
            return;
        }
    }

    switch (gDuel_bSceneSubStep & 0xF) {
    case 1:
        DisplayObject_InterpolatePositionCosine(
            DISPLAY_OBJECT_POSITION_VIEW(object),
            0x86, 0x2A, object->field_60);
        object->field_60 += 0x80;
        if (object->field_60 < 0x800) {
            break;
        }
        object->field_30.h.field_30 = 0x86;
        object->field_30.h.field_32 = 0x2A;
        gDuel_bSceneSubStep = 2;
        /* Fall through to the next presentation phase in the same frame. */
    case 2:
        if (object->field_68 >= 0x14) {
            goto state_four;
        }
        gDuel_bSceneSubStep = 3;
        /* Fall through to initialize the choice dialog. */
    case 3:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            gDuel_wSelectedCardID =
                D_8015C424_cards.field_cards[object->field_6A].card_id;
#ifdef VERSION_JAPAN
            box = TextBox_CreateFlagged(0, 0x21, 0x50, 0x6E, 0xA0, 0x30, 0x20);
#elif defined(VERSION_EUROPE)
            /* The European build makes neither the func_8003B6AC call nor
               the two field stores, and flags the box 0x40. */
            box = TextBox_CreateFlagged(0, 0x21, 0x48, 0x6E, 0xB0, 0x30, 0x40);
#else
            func_8003B6AC(0, 0xB);
            box = TextBox_CreateFlagged(0, 0x21, 0x48, 0x6E, 0xB0, 0x30, 0x20);
            box->glyph_advance_x = 8;
            box->line_advance_y = 0x10;
#endif
            do {
                TextBox_UpdateChannels();
            } while (box->field_30 == 0);
            break;
        }
        if (gDuel_bSceneSubStep & 0x10) {
            gDuel_aCardRecords[object->field_6A].flags &= ~0x200;
            if (gDialog_bChoice) {
                gDuel_aCardRecords[object->field_6A].flags |= 0x200;
            }
            TextBox_Destroy(gText_aChannels);
state_four:
            gDuel_bSceneSubStep = 4;
            break;
        }
        if (!Dialog_ReadChoiceInput(gText_aChannels) &&
            (gInput_wPad1Pressed & DUEL_CARD_STAGING_CONFIRM_MASK)) {
            SD_SEPlayFull(7);
            gDuel_bSceneSubStep |= 0x10;
        }
        break;
    case 4:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            object->field_2C.h.field_2E = Duel_ProjectCardSlotScreenX(gDuel_bCardMoveSlot) - 0x1E;
            DisplayObject_SavePosition(DISPLAY_OBJECT_SNAPSHOT_VIEW(object));
            object->field_60 = 0x400;
        }
        if (!(gDuel_bSceneSubStep & 0x40)) {
            Widget_SlideSine(DISPLAY_OBJECT_POSITION_VIEW(object),
                             object->field_2C.h.field_2E, -0xBC,
                             object->field_60);
            object->field_60 -= 0x2A;
            if (object->field_60 > 0) {
                break;
            }
            gDuel_bSceneSubStep |= 0x40;
            scratch = (DuelCardRecord *)0x1F800000;
            *scratch = gDuel_aCardRecords[object->field_6A];
            Duel_SetupFieldCardFromDeck(gDuel_bCardMoveSlot, object->field_6B);
            card = &gDuel_aCardRecords[gDuel_bCardMoveSlot];
            flags = card->flags | (scratch->flags & 0x7E00);
            card->flags = flags & ~0x400;
            if (!(flags & 0x1000)) {
                card->flags = flags & ~0x2400;
            }
            card->stat_modifier = scratch->stat_modifier;
            DisplayObject_ReleaseIfPresent(object);
            gDisplayObject_apWorkSlots[0] = card->object;
            object = gDisplayObject_apWorkSlots[0];
            Duel_ApplyCardObjectFlags((DuelCardDisplayObject *)object);
            DISPLAY_OBJECT_POSITION_VIEW(object)->out_y = DUEL_CARD_OFFSCREEN_Y;
            DisplayObject_SavePosition(DISPLAY_OBJECT_SNAPSHOT_VIEW(object));
            object->field_60 = -0x400;
            break;
        }
        Widget_SlideSine(DISPLAY_OBJECT_POSITION_VIEW(object),
                         (s16)object->field_30.h.field_30, -0x18,
                         object->field_60);
        object->field_60 += 0x2A;
        if (object->field_60 < 0) {
            break;
        }
        DISPLAY_OBJECT_POSITION_VIEW(object)->out_y = -0x18;
        gDuel_bSceneSubStep = 5;
        SD_SEPlayFull(0xC);
        break;
    case 5:
        if (gDuel_wPendingEquipBonus &&
            Duel_SelectTrapByCardId(DUEL_REVERSE_TRAP_CARD_ID)) {
            gDuel_bSceneSubStep = 6;
            break;
        }
        gDuel_wSceneStateFlags = 5;
        break;
    case 6:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0xC0;
            gDuel_wTrapPresentationMode = 0;
        }
        if (gDuel_bSceneSubStep & 0x40) {
            if (Duel_UpdateTrapPresentation()) {
                break;
            }
            request = DuelEffect_CreateRequest(0xD);
            request->field_00 = object->field_30.h.field_30;
            request->field_02 = object->field_30.h.field_32;
            z = object->field_34.h.field_34;
            request->field_12 = -gDuel_wPendingEquipBonus;
            request->field_04 = z;
            SD_SEPlayFull(0x21);
            gDuel_bSceneSubStep &= ~0x40;
            break;
        }
        slot = object->field_6A;
        gDuel_wSceneStateFlags = 5;
        card = &gDuel_aCardRecords[slot];
        card->stat_modifier -= gDuel_wPendingEquipBonus * 2;
        break;
    }
}
