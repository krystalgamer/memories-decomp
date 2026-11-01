#define FUNC_80018004_AMBIENT_POSITION_ARGS
#define GDUEL_WBGMID_IN_DATA
#define D_8009B1B9_AS_SIGNED
#include "../types.h"
#include "duel_init_scene.h"
#include "display_object.h"
#include "duel_card.h"
#include "card_presentation.h"
#include "duel_card_object_helpers.h"
#include "duel_effect_request.h"
#include "duel_card_record_lifecycle.h"
#include "duel_card_staging.h"
#include "duel_side_state.h"
#include "duel_scene_state.h"
#include "duel_phase_entry.h"
#include "duel_scene_resume.h"
#include "duel_scene_card_placement.h"
#include "duel_presentation_controls.h"
#include "view_state.h"
#include "card_constants.h"
#include "duel_scene_state.h"
#include "duel_hand.h"
#include "duel_deck_card.h"
#include "duel_rank.h"
#include "fade.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "duel_selection_layout.h"

#include "duel_card_layout.h"
#include "duel_grid.h"
#include "sound.h"
#include "../unmatched.h"
#include "duel_field_card_render_data.h"
#include "duel_card_display_objects.h"
#include "duel_deck_card_data.h"
#include "duel_swords_effect.h"
#include "view_state_current.h"
#include "duel_battle_removal_slots.h"

/* Regional values: the cursor's field_0C (0xAE -> 0xBE), the base of
 * field_40 (0x2E0 -> 0x360) and the y Duel_CreateHandCardDisplayObject places a hand card at
 * (0x292 -> 0x2A2). The European build (src/game/european/) defines its own. */
#ifndef DUEL_PHASE_CURSOR_FIELD_0C
#define DUEL_PHASE_CURSOR_FIELD_0C 0xAE
#endif
#ifndef DUEL_PHASE_FIELD_40_BASE
#define DUEL_PHASE_FIELD_40_BASE 0x2E0
#endif
#ifndef DUEL_PHASE_HAND_CARD_Y
#define DUEL_PHASE_HAND_CARD_Y 0x292
#endif

#define DUEL_CARD_DISPLAY_OBJECT_VIEW(object) \
    ((DuelCardDisplayObject *)(object))

/* Three contiguous entries from the gDuel_apfnSceneStateHandler duel-phase callback table:
   resume/replay reconstruction, initial deck and selection setup, and draw
   phase hand reconstruction. All three use gDuel_wSceneStateFlags as their first-call
   latch and rebuild active duel-card and side state around fade or transfer
   gates. */

void DuelScene_UpdateResume(void)
{
    DuelCardRecord *rec;
    DuelEffectRequest *obj;
    u8 *b;
    DisplayObject *card;
    s32 i;
    s32 keep;
    s32 y;
    u16 flags;
    s8 n;

    if ((gDuel_wSceneStateFlags & DUEL_SCENE_FLAG_INITIALIZED) == 0) {
        gDuel_wSceneStateFlags |= DUEL_SCENE_FLAG_INITIALIZED;
        rec = gDuel_aFieldCardRecords;
        for (i = HAND_SIZE; i < DUEL_CARD_SIDE_RECORD_COUNT; i++, rec++) {
            flags = rec->flags;
            if (flags & DUEL_CARD_FLAG_OCCUPIED) {
                keep = flags & 0x7A00;
                y = rec->stat_modifier;
                Duel_SetupFieldCardFromDeck(
                    i, ((DuelDeckCardRecord *)rec->data)->deck_index);
                rec->flags |= keep;
                rec->stat_modifier = y;
                Duel_ApplyCardObjectFlags(
                    DUEL_CARD_DISPLAY_OBJECT_VIEW(rec->object));
            }
        }
        rec = gDuel_aFieldCardRecords + 15;
        for (i = 20; i < DUEL_CARD_RECORD_COUNT; i++, rec++) {
            flags = rec->flags;
            if (flags & DUEL_CARD_FLAG_OCCUPIED) {
                keep = flags & 0x7A00;
                y = rec->stat_modifier;
                Duel_SetupFieldCardFromDeck(
                    i, ((DuelDeckCardRecord *)rec->data)->deck_index);
                rec->flags |= keep;
                rec->stat_modifier = y;
                Duel_ApplyCardObjectFlags(
                    DUEL_CARD_DISPLAY_OBJECT_VIEW(rec->object));
            }
        }
        ViewState_ApplyOrbit();
        for (i = 0; i < DUEL_SIDE_COUNT; i++) {
            if (gDuel_aSideStates[i].swords_turns_remaining != 0) {
                obj = DuelEffect_AllocateRequest(0x15);
                obj->phase = i + 2;
                obj->flags |= DUEL_EFFECT_REQUEST_FLAG_NONBLOCKING;
                gDuel_apSwordsEffectObjects[i] = obj;
            }
        }
        if (gDuel_pActiveSideState->rank.result_adjustment ==
            DUEL_RANK_ADJUST_EXODIA_WIN) {
            gDuel_wSceneStateFlags |= 0x2000;
            for (i = 0; i < DUEL_FIELD_SIDE_ZONE_COUNT; i++) {
                rec = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices[
                    i + gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT]];
                if (rec->flags & DUEL_CARD_FLAG_OCCUPIED) {
                    DuelCard_RemoveFromField(rec);
                }
            }
        } else {
            if (gDuel_aSideStates[0].life_points.signed_value != 0 &&
                gDuel_aSideStates[1].life_points.signed_value != 0) {
                SD_BGMPlay(gDuel_wBgmId);
            }
        }
        Fade_StartIn();
        return;
    }

    if ((gDuel_wSceneStateFlags & 0x4000) == 0) {
        if (((gFile_dwTransferState_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             gFile_dwCancelRequestState_abs) != 0) {
            return;
        }
        if ((gFade_State.flags & FADE_FLAG_ACTIVE) != 0) {
            return;
        }
        gDuel_wSceneStateFlags |= 0x4000;
        gDuel_bPresentationCursor = 2;
        if ((gDuel_wSceneStateFlags & 0x2000) != 0) {
            gDuel_wSceneStateFlags = 0xC;
        }
        return;
    }

    for (;;) {
        n = gDuel_bPresentationCursor - 1;
        gDuel_bPresentationCursor = n;
        if (n < 0) {
            gDuel_wSceneStateFlags = 5;
            return;
        }
        if (gDuel_bBattleRemovalSlot0[n] >= 0) {
            break;
        }
    }
    {
        s32 replay_offset = DUEL_CARD_STAGING_REPLAY_BASE_OFFSET;
        DuelCardReplayRecordBlock *replay;

        b = gDuel_abCardStagingBuffer;
        replay = (DuelCardReplayRecordBlock *)(
            (u8 *)&((DuelCardRecord *)b)[gDuel_bBattleRemovalSlot0[n]] + replay_offset);
        card = replay->record.object;
    }
    ViewState_ApplyOrbit();
    obj = DuelEffect_CreateRequest(0xB);
    obj->field_00 = card->field_30.h.field_30;
    obj->field_02 = card->field_30.h.field_32;
    obj->field_04 = *(u16 *)&card->field_34;
    obj->phase = Duel_GetCardEffectVariant((CardObject *)card);
    DuelCard_RemoveFromField(&gDuel_aCardRecords[card->field_6A]);
    SD_SEPlayFull(0x1F);
}

void DuelScene_UpdateStartup(void)
{
    u16 *w;
    DuelDeckCardRecord *rec;
    s32 i;
    s32 atk;
    s32 def;
    s32 stat;
    s32 stat2;

    w = (u16 *)&gViewState_Current;
    if ((gDuel_wSceneStateFlags & DUEL_SCENE_FLAG_INITIALIZED) == 0) {
        gDuel_wSceneStateFlags |= DUEL_SCENE_FLAG_INITIALIZED;
        Duel_RequestCombinedDeckData();
        gViewState_Current.field_00 = 0x4B0;
        w[2] = 0x358;
        w[1] = 0x16C0;
        ViewState_ApplyOrbit();
        gDuel_bSceneSubStep = 2;
        Fade_StartIn();
        return;
    }

    switch (gDuel_bSceneSubStep & 0x1F) {
    case 2:
        gViewState_Current.field_00 -= 2;
        w[2] -= 2;
        w[1] -= 0x10;
        if ((s16)w[1] <= 0x400) {
            gViewState_Current.field_00 = 0x258;
            w[2] = 0x100;
            w[1] = 0x400;
            gDuel_bSceneSubStep = 3;
        }
        ViewState_ApplyOrbit();
        break;
    case 3:
        if ((gDuel_bSceneSubStep & 0x80) == 0) {
            gDuel_bSceneSubStep |= 0x80;
            gDuel_wDisplayTransitionFlags = 2;
            gDuel_pActiveSelectionRecord = (DuelSelectionRecord *)(gDuel_abSelectionSides +
                gDuel_bActiveSide * DUEL_SELECTION_SIDE_SIZE);
            *(u16 *)&gDuel_pActiveSelectionRecord->field_0C = DUEL_PHASE_CURSOR_FIELD_0C;
        }
        if (gDuel_wDisplayTransitionFlags == 0) {
            gDuel_bSceneSubStep = 4;
        }
        break;
    case 4: {
        u32 mask = FILE_TRANSFER_REQUEST_BLOCKED_MASK;

        if (((gFile_dwTransferState_abs & mask) |
             gFile_dwCancelRequestState_abs) != 0) {
            break;
        }
        SD_BGMPlay(gDuel_wBgmId);
        Duel_PopulateCombinedDeckData();
        atk = 0;
        def = 0;
        rec = gDuel_aDeckCardRecords;
        for (i = 0; i < DECK_SIZE; i++, rec++) {
            stat = gDuel_adwCardStats[rec->id - 1];
            atk += (stat & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
            def += ((stat >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) *
                   CARD_STAT_SCALE;
        }
        gDuel_aSideStates[0].field_0E = atk / DECK_SIZE;
        gDuel_aSideStates[0].field_10 = def / DECK_SIZE;
        atk = 0;
        def = 0;
        rec = &gDuel_aDeckCardRecords[DECK_SIZE];
        for (i = 0; i < DECK_SIZE; i++, rec++) {
            stat2 = gDuel_adwCardStats[rec->id - 1];
            atk += (stat2 & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
            def += ((stat2 >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) *
                   CARD_STAT_SCALE;
        }
        gDuel_aSideStates[1].field_0E = atk / DECK_SIZE;
        gDuel_aSideStates[1].field_10 = def / DECK_SIZE;
        gDuel_bSceneSubStep = 5;
        break;
    }
    case 5:
        Duel_ClearHandSlots();
        gDuel_bPendingDrawCount = HAND_SIZE;
        gDuel_wSceneStateFlags = 3;
        ((DuelSelectionRecord *)(gDuel_abSelectionSides +
            gDuel_bActiveSide * DUEL_SELECTION_SIDE_SIZE))->hand = gDuelHand_aSlots;
        break;
    }
}

/* Draw phase entry. On its first call it sets the phase flag, points the
   side state and selection records at the current side, ticks down the
   side's Swords counter (releasing its effect object when
   it reaches zero), resets the used flag on every card record, then
   rebuilds the hand: the five hand slot bytes are copied out and cleared,
   each valid one gets its card record set up and a hand object spawned at
   a fixed x and a 0x3C-spaced y, and the number of empty slots is stored
   for the draw. Later calls only clear the phase flag once the message
   state has returned to zero. */
void DuelScene_UpdateDrawPhase(void) {
    u8 hand[HAND_SIZE];
    DuelCardRecord *rec;
    u8 *fl;
    DuelCardRecord *placed;
    DuelHandSlot *base;
    DuelHandSlot *slot;
    u8 *p;
    s32 side;
    s32 i;
    s32 n;
    s32 idx;
    s32 y;
    u16 flags;
    s8 c;

    if ((gDuel_wSceneStateFlags & DUEL_SCENE_FLAG_INITIALIZED) == 0) {
        gDuel_wSceneStateFlags |= DUEL_SCENE_FLAG_INITIALIZED;
        *(u16 *)&gDuel_pLifePointsPanel->field_40.h.field_40 =
            (gDuel_bActiveSide << 4) | DUEL_PHASE_FIELD_40_BASE;
        Duel_ClearHandSlots();
        side = gDuel_bActiveSide;
        gDuel_pActiveSideState = &gDuel_aSideStates[side];
        gDuel_pActiveSelectionRecord = (DuelSelectionRecord *)(gDuel_abSelectionSides +
                side * DUEL_SELECTION_SIDE_SIZE);
        base = gDuelHand_aSlots;
        gDuel_pActiveSelectionRecord->hand = base;
        if (gDuel_pActiveSideState->swords_turns_remaining != 0) {
            c = gDuel_pActiveSideState->swords_turns_remaining - 1;
            gDuel_pActiveSideState->swords_turns_remaining = c;
            if (c <= 0) {
                gDuel_pActiveSideState->swords_turns_remaining = 0;
                gDuel_apSwordsEffectObjects[gDuel_bActiveSide]->phase =
                    0xFFFD - gDuel_bActiveSide;
                gDuel_apSwordsEffectObjects[gDuel_bActiveSide] = 0;
            }
        }
        gDuel_pActiveSideState->rank.turns_taken++;
        rec = gDuel_aCardRecords;
        for (i = 0; i < DUEL_CARD_RECORD_COUNT; i++, rec++) {
            flags = rec->flags;
            if (flags & DUEL_CARD_FLAG_OCCUPIED) {
                rec->flags = flags & ~DUEL_CARD_FLAG_USED_THIS_TURN;
                Duel_ApplyCardObjectFlags(
                    DUEL_CARD_DISPLAY_OBJECT_VIEW(rec->object));
            } else {
                rec->flags = 0;
            }
        }
        for (i = 0; i < HAND_SIZE; i++) {
            hand[i] = gDuel_pActiveSideState->hand[i];
            gDuel_pActiveSideState->hand[i] = -1;
        }
        n = 0;
        i = n;
        slot = base;
        y = 0xE;
        idx = gDuel_bActiveSide * DUEL_CARD_SIDE_RECORD_COUNT;
        rec = &gDuel_aCardRecords[idx];
        placed = rec;
        for (; i < HAND_SIZE; i++, rec++) {
            p = &hand[i];
            rec->flags = 0;
            rec->object = 0;
            if (*(s8 *)p >= 0) {
                gDuel_pActiveSideState->hand[n] = *p;
                Duel_SetupCardRecord(idx, *(s8 *)p);
                idx++;
                n++;
                slot->object = (DisplayObject *)Duel_CreateHandCardDisplayObject(placed, y, DUEL_PHASE_HAND_CARD_Y);
                slot++;
                y += 0x3C;
                placed++;
            }
        }
        gDuel_bPendingDrawCount = HAND_SIZE - n;
        gDuel_wDisplayTransitionFlags = 2;
        gDuel_pActiveSelectionRecord = (DuelSelectionRecord *)(gDuel_abSelectionSides +
                gDuel_bActiveSide * DUEL_SELECTION_SIDE_SIZE);
        *(u16 *)&gDuel_pActiveSelectionRecord->field_0C = DUEL_PHASE_CURSOR_FIELD_0C;
    } else if (gDuel_wDisplayTransitionFlags == 0) {
        gDuel_wSceneStateFlags = 3;
    }
}
