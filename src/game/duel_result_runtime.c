#define D_8009B0CC_IN_DATA
#define D_8009B362_IN_DATA
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#define GRAPHICS_VIEWPORT_IN_DATA
#define D_8009B360_AS_SIDE_ARRAY
#define D_8009B34E_IN_DATA
#define D_8009B355_IN_DATA
#define GDUEL_WSELECTEDCARDID_IN_DATA
#define GINPUT_PAD1_REPEAT_IN_DATA
#define TEXT_STRING_ID_IN_DATA
#define DUEL_SAVE_WINDOWS_AS_PAIR
#define DUEL_RITUAL_DATA_RESULT_VIEW
#include "../types.h"
#include "text_box_blocking_build.h"
#include "display_object.h"
#include "display_object_core.h"
#include "display_object_lifecycle.h"
#include "graphics_frame.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object_helpers.h"
#include "display_object_layout.h"
#include "duel_card_display_state.h"
#include "duel_scene_state.h"
#include "duel_check_ritual.h"
#include "../psyq/rand.h"
#include "display_object_motion.h"
#include "input.h"
#include "rand_get_interval.h"
#include "trig_constants.h"
#include "duel_init_scene.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "duel_result_load_outro_package_stage.h"
#include "sound.h"
#include "sound_output.h"
#include "fade.h"
#include "duel_side_state.h"
#include "ai_controller_handoff.h"
#include "ai_opponent_data.h"
#include "duel_package.h"
#include "duel_screen_tables.h"
#include "duel_result_outro.h"
#include "save_data.h"
#include "card_constants.h"
#include "duel_scene_state.h"
#include "duel_grid.h"
#include "duel_rank.h"
#include "duel_result_display.h"
#include "duel_rewards.h"
#include "text_staging.h"
#include "text_message_staging.h"
#include "text_box_lifecycle.h"
#include "text_box_runtime.h"
#include "display_object_config.h"
#include "text_constants.h"
#include "display_asset_banks.h"
#include "view_state.h"
#include "duel_check_quit_input.h"
#include "duel_effect.h"
#include "../unmatched.h"
#include "display_resource_banks.h"
#include "view_state_current.h"
#ifdef VERSION_EUROPE
#include "duel_effect_resource_setup.h"
#endif

#ifdef DUEL_RESULT_LOCALIZED_SPRITES
#include "duel_result_localized_sprites.h"
#define DUEL_RESULT_OUTRO_SPRITE_LIMIT 10
#define DUEL_RESULT_OUTRO_ACTIVE_COUNT gDuelResult_nOutroSpriteCount
#else
#define DUEL_RESULT_OUTRO_SPRITE_LIMIT DUEL_RESULT_SPRITE_COUNT
#define DUEL_RESULT_OUTRO_ACTIVE_COUNT DUEL_RESULT_SPRITE_COUNT
#endif

#define DUEL_RESULT_ORBIT_ANGLE_STEP 0x30

/* The complete duel-result outro, rank, drop, award, and rewards-screen runtime.

   The duel-result outro and the two update callbacks it hangs on its
   confetti: DuelResult_UpdateOrbitSprite orbits a sprite around its spawn
   point, DuelResult_UpdateDepartingSprite sends it flying off, and
   DuelScene_UpdateResultOutro is the outro sequence that spawns the sprites
   on the first and retargets them at the second. The three sources were
   recorded at gcc_2_8_1_g8_no_split,
   gcc_2_8_1_g8 and gcc_2_8_1_g8_split; all nine functions compile at
   gcc_2_8_1_g8_split. Bounded below by the outro package's transfer callback
   DuelResult_LoadOutroPackageStage, which cannot reproduce its object under this unit's
   split profile, and above by the debug effect screen at DebugEffect_UpdateViewControls. */

/* Per-frame update for an object that orbits a fixed base position
   (field_2C/field_2E) at DUEL_RESULT_ORBIT_ANGLE_STEP per frame, with radius
   the object's field_28 (which also serves as its countdown timer -- it decays
   by 2/frame). When the timer reaches 0: clears field_6C/update, snaps the
   current position (field_30/field_32) back to the base position in one 32-bit
   copy, and returns. Otherwise, while gGraphics_dwSyncedFrameCount's bit 0 is set, spawns a
   companion slot object at the object's current position (tagged via
   sub_table_lookup_set_flag using depth_offset-1), then advances the angle and
   recomputes the orbit position from base + (rcos,rsin)*radius/ONE. */

void DuelResult_UpdateOrbitSprite(DisplayObject *arg0) {
    s16 timer;
    u16 angle;
    DisplayObject *slot;
    s32 vy;

    timer = (s16)arg0->position.h.field_28 - 2;
    arg0->position.h.field_28 = timer;
    if (timer <= 0) {
        arg0->field_6C = 0;
        arg0->update = 0;
        arg0->field_30.word = arg0->field_2C.word;
        return;
    }

    if (gGraphics_dwSyncedFrameCount & 1) {
        slot = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        if (slot != 0) {
            DisplayObject_ConfigureSpriteAtPositionWithResource(slot, (s16)arg0->field_30.h.field_30,
                                   (s16)arg0->field_30.h.field_32, 0,
                                   arg0->field_68, arg0->field_69, 0x11, 9,
                                   gDisplay_abResourceBank0);
            slot->field_40.h.field_40 = (u16)arg0->field_40.h.field_40 + 0x80;
            slot->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                           DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
            slot->attribute |= (GsALON | GsAONE);
            DisplayObject_SetDepthOffset(slot, (u8)arg0->depth_offset - 1);
            slot->field_60 = 8;
            slot->update = (DisplayObjectCallback)DisplayObject_FadeBrightnessAndRelease;
        }
    }

    angle = arg0->position.h.field_2A + DUEL_RESULT_ORBIT_ANGLE_STEP;
    arg0->position.h.field_2A = angle;
    arg0->field_30.h.field_30 =
        (s16)arg0->field_2C.h.field_2C +
        rcos((s16) angle) * (s16)arg0->position.h.field_28 / ONE;
    vy = rsin((s16) arg0->position.h.field_2A) * (s16)arg0->position.h.field_28;
    arg0->field_30.h.field_32 = arg0->field_2C.h.field_2E + vy / ONE;
}

void DuelResult_UpdateDepartingSprite(DuelCardDisplayObject *object)
{
    if (DisplayObject_MarkInitialized((DisplayObjectLifecycle *)object) == 0) {
        object->flags |= DISPLAY_OBJECT_FLAG_CLIP_TEST;
    }
    object->field_21 -= 2;
    if (object->field_21 < 0xC0) {
        DisplayObject_ReleaseIfPresent((u8 *)object);
    }
}

#ifndef DUEL_RESULT_OUTRO_START_SECTOR
#define DUEL_RESULT_OUTRO_START_SECTOR FILE_WA_DUEL_RESULTS_START_SECTOR
#endif

#ifndef DUEL_RESULT_DUELIST_DATA_FIRST_SECTOR
#define DUEL_RESULT_DUELIST_DATA_FIRST_SECTOR DUELIST_DATA_FIRST_SECTOR
#endif

/* Duel-result outro sequence, driven from the scene state word gDuel_wSceneStateFlags.

   The first call (bit 0x8000 clear) fades the BGM out, records the winning
   side in gDuel_bPlayerLost/gDuel_bOutroWinnerSide, requests the outro package (0x1DAB, 0x22
   sectors) through DuelResult_LoadOutroPackageStage, picks the win or lose track for
   gDuelResult_wBgmId -- 0x72F0 only when side 1 won (the player lost) against
   a real opponent, otherwise 0x72E0 --
   and hands gDuel_pTerrainIcon and gDuel_pLifePointsPanel to DisplayObject_UpdateCosineSlide
   before entering step 1 of gDuel_bSceneSubStep.

   Afterwards the low nibble of gDuel_bSceneSubStep is the step and bit 0x80 marks
   "step already entered":

     1  wait for the DMA/queue flags to drain, then reload the selected
        three-sector duelist data block into 0x801781D8. The adjacent
        SD_BGMPreload call handles gDuelResult_wBgmId separately; once the transfer
        has drained, SD_BGMPlay starts that track.
     2  spawn the seven confetti sprites from the gDuelResult_aSpriteSpecsOpponent (real opponent)
        or gDuelResult_aSpriteSpecsNoOpponent (no opponent) table row for the winning side, each on
        DuelResult_UpdateOrbitSprite with a random radius and orbit key, and
        remember them in the gDuel_awRitualData slot table; then wait for
        DisplayObject_FindAllocatedByTag.
     3  hold for 0x258 frames or until the player presses one of the 0xE0
        buttons, then retarget every spawned sprite at DuelResult_UpdateDepartingSprite so it
        flies off, and wait for DisplayObject_FindAllocatedByTag again.
     4  once the fade at gFade_State.flags has finished, hand the scene over to
        state 0xD.  */

void DuelScene_UpdateResultOutro(void)
{
    DuelResultSpriteSlot *slots;
    DisplayObject *obj;
    DuelResultSpriteSpec *spec;
    s32 i;
    s32 flags;
    s32 state;
    s32 id;
    s32 timer;
    s32 mode;
    s32 v;

    slots = (DuelResultSpriteSlot *)&gDuel_awRitualData;

    v = gDuel_wSceneStateFlags;
    if ((v & DUEL_SCENE_FLAG_INITIALIZED) == 0) {
        gDuel_wSceneStateFlags = v | DUEL_SCENE_FLAG_INITIALIZED;
        SD_BGMFadeOut();
        id = gDuel_bWinnerSide;
        gDuel_bPlayerLost = 0;
        if (id != 0) {
            gDuel_bPlayerLost = 1;
        }
        gDuel_bOutroWinnerSide = id;
        File_RequestAsyncTransfer(
            0, 0,
            DUEL_RESULT_OUTRO_START_SECTOR,
            FILE_WA_DUEL_RESULTS_SECTOR_COUNT,
            DuelResult_LoadOutroPackageStage, 0, 0
        );
        mode = 0x72E0;
        if (gDuel_bWinnerSide != 0) {
            if (gDuel_bOpponentID >= 0) {
                mode = 0x72F0;
            }
        }
        obj = gDuel_pTerrainIcon;
        gDuelResult_wBgmId = mode;
        /* The record spells 0x28 u16 and this is the one site that puts a
           negative value there. Through the plain member GCC materialises
           -116 as `ori 0xff8c` where retail has `addiu -116`; the store at
           0x198 below is unaffected, so the divergence is the constant's
           sign, not the member. */
        *(s16 *)&obj->position.h.field_28 = -116;
        obj->field_2C.h.field_2C = 0x30;
        obj->field_6C = 1;
        obj->update = (DisplayObjectCallback)DisplayObject_UpdateCosineSlide;
        obj->position.h.field_2A = (s16)obj->field_30.h.field_32;
        obj = gDuel_pLifePointsPanel;
        obj->position.h.field_28 = 0x198;
        obj->field_2C.h.field_2C = 0x30;
        obj->field_6C = 1;
        obj->update = (DisplayObjectCallback)DisplayObject_UpdateCosineSlide;
        gDuel_bSceneSubStep = 1;
        obj->position.h.field_2A = (s16)obj->field_30.h.field_32;
        return;
    }

    if (gDuel_wDisplayTransitionFlags != 0) {
        return;
    }

    flags = gDuel_bSceneSubStep;
    state = flags & 0xF;
    switch (state) {
    case 1:
        if ((flags & 0x80) == 0) {
            if ((gFile_dwTransferState_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                gFile_dwCancelRequestState_abs) {
                return;
            }
            gDuel_bSceneSubStep = flags | 0x80;
            gDuel_wDisplayTransitionFlags = 8;
            id = gDuel_bOpponentID;
            if (id < 0) {
                id = 1;
            }
            File_RequestAsyncTransfer(
                0, 0,
                id * DUELIST_DATA_SECTOR_COUNT +
                    DUEL_RESULT_DUELIST_DATA_FIRST_SECTOR,
                DUELIST_DATA_SECTOR_COUNT, 0, 0,
                (s32)((u8 *)slots - 0x1800));
            SD_BGMPreload(gDuelResult_wBgmId);
        } else {
            if ((gFile_dwTransferState_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                gFile_dwCancelRequestState_abs) {
                return;
            }
            gDuel_bSceneSubStep = 2;
            SD_BGMPlay(gDuelResult_wBgmId);
        }
        break;
    case 2:
        if ((flags & 0x80) == 0) {
            gDuel_bSceneSubStep = flags | 0x80;
            for (i = 0; i < DUEL_RESULT_OUTRO_SPRITE_LIMIT; i++) {
#ifdef DUEL_RESULT_LOCALIZED_SPRITES
                /* Fixed-language builds keep the selector in 1..4. */
                if (gText_bLanguageIndex == 1) {
                    if (gDuel_bOpponentID >= 0) {
                        spec = &gDuelResult_aFrenchSpriteSpecsOpponent[gDuel_bWinnerSide][i];
                    } else {
                        spec = &gDuelResult_aFrenchSpriteSpecsNoOpponent[gDuel_bWinnerSide][i];
                    }
                } else if (gText_bLanguageIndex == 2) {
                    if (gDuel_bOpponentID >= 0) {
                        spec = &gDuelResult_aGermanSpriteSpecsOpponent[gDuel_bWinnerSide][i];
                    } else {
                        spec = &gDuelResult_aGermanSpriteSpecsNoOpponent[gDuel_bWinnerSide][i];
                    }
                } else if (gText_bLanguageIndex == 3) {
                    if (gDuel_bOpponentID >= 0) {
                        spec = &gDuelResult_aItalianSpriteSpecsOpponent[gDuel_bWinnerSide][i];
                    } else {
                        spec = &gDuelResult_aItalianSpriteSpecsNoOpponent[gDuel_bWinnerSide][i];
                    }
                } else if (gText_bLanguageIndex == 4) {
                    if (gDuel_bOpponentID >= 0) {
                        spec = &gDuelResult_aSpanishSpriteSpecsOpponent[gDuel_bWinnerSide][i];
                    } else {
                        spec = &gDuelResult_aSpanishSpriteSpecsNoOpponent[gDuel_bWinnerSide][i];
                    }
                }
                if (spec->x == 0) {
                    continue;
                }
                gDuelResult_nOutroSpriteCount = i + 1;
#else
                if (gDuel_bOpponentID >= 0) {
                    spec = &gDuelResult_aSpriteSpecsOpponent[gDuel_bWinnerSide][i];
                } else {
                    spec = &gDuelResult_aSpriteSpecsNoOpponent[gDuel_bWinnerSide][i];
                }
#endif
                slots[i].object = 0;
                if (spec->kind != 0) {
                    obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
                    DisplayObject_ConfigureSpriteAtPositionWithResource(obj, spec->x, spec->y, 0,
                                  gDuel_bWinnerSide, spec->kind, 0x11, 9,
                                  gDisplay_abResourceBank0);
                    if (obj->field_69 >= 0x1A) {
                        obj->field_40.h.field_40 =
                            (u16)obj->field_40.h.field_40 + 0x10;
                    }
                    obj->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                                  DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
                    obj->attribute |= (GsALON | GsAONE);
                    obj->field_48.h.field_48 = spec->tag;
                    obj->field_48.h.field_4A = 0x18;
                    DisplayObject_SelectOrderingTable1(obj);
                    obj->field_2C.word = obj->field_30.word;
                    obj->position.h.field_28 = (rand() & 0x3F) + 0x140;
                    obj->position.h.field_2A =
                        Rand_GetInterval(TRIG_ANGLE_FULL_TURN);
                    obj->field_6C = 1;
                    obj->update =
                        (DisplayObjectCallback)DuelResult_UpdateOrbitSprite;
                    slots[i].object = obj;
                }
            }
        } else {
            if ((gFile_dwTransferState_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                gFile_dwCancelRequestState_abs) {
                return;
            }
            if (DisplayObject_FindAllocatedByTag(1) == 0) {
                gDuel_bSceneSubStep = 3;
            }
        }
        break;
    case 3:
        if ((flags & 0x80) == 0) {
            gDuel_bSceneSubStep = flags | 0x80;
            gDuel_wStepCounter = 0x258;
        }
        if ((gDuel_bSceneSubStep & 0x40) == 0) {
            timer = gDuel_wStepCounter - 1;
            gDuel_wStepCounter = timer;
            if ((s16)timer > 0) {
                if ((gInput_wPad1Pressed &
                     (PAD_BUTTON_CANCEL | PAD_BUTTON_CONFIRM_MASK)) == 0) {
                    if ((SD_GetStatusFlags() & 0x80) != 0) {
                        return;
                    }
                }
            }
            gDuel_bSceneSubStep = gDuel_bSceneSubStep | 0x40;
            for (i = 0; i < DUEL_RESULT_OUTRO_ACTIVE_COUNT; i++) {
                obj = slots[i].object;
                if (obj != 0) {
                    obj->field_6C = 1;
                    obj->update = (DisplayObjectCallback)DuelResult_UpdateDepartingSprite;
                }
            }
        } else {
            if (DisplayObject_FindAllocatedByTag(1) == 0) {
                gDuel_bSceneSubStep = 4;
            }
        }
        break;
    case 4:
        if ((flags & 0x80) == 0) {
            gDuel_bSceneSubStep = flags | 0x80;
        }
        if ((gFade_State.flags & FADE_FLAG_ACTIVE) == 0) {
            gDuel_wSceneStateFlags = 0xD;
        }
        break;
    }
}

/* The duel's reward step: Duel_ShowResultPage shows one of the three result
   pages and opens its text box, Duel_CalcRankScoreChange is the per-rule
   lookup Duel_CalcRankScore sums into that display's side scores, and the
   card drop and award follow. The first two were recorded at gcc_2_8_1_g8
   and gcc_2_8_1_g0_split and compile to identical objects at this unit's
   gcc_2_8_1_g8_split. */

/* Regional values: the result page's text box y and height (0x28/0x120 ->
   0x38/0xF0). The European build (src/game/european/) defines its own. */
#ifndef DUEL_RESULT_PAGE_TEXT_Y
#define DUEL_RESULT_PAGE_TEXT_Y 0x28
#endif
#ifndef DUEL_RESULT_PAGE_TEXT_HEIGHT
#define DUEL_RESULT_PAGE_TEXT_HEIGHT 0x120
#endif

void Duel_ShowResultPage(s32 page)
{
    s32 i;
    DisplayObject *child;
    void *object;

    DisplayObject_SetResourceVariant((DisplayObjectConfig *)gDuel_pResultDisplayState->root, page);
    if (page == 0) {
        DuelResultDisplayState *state = gDuel_pResultDisplayState;

        for (i = 0; i < DUEL_RESULT_DISPLAY_CHILD_COUNT; i++) {
            child = state->children[i];
            if (child == 0) {
                break;
            }
            child->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
        }
    } else {
        DuelResultDisplayState *state = gDuel_pResultDisplayState;

        for (i = 0; i < DUEL_RESULT_DISPLAY_CHILD_COUNT; i++) {
            child = state->children[i];
            if (child == 0) {
                break;
            }
            child->flags &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
        }
    }
    object = TextBox_Create(
        0, gDuel_pResultDisplayState->page_text_ids[page], 0x1A, DUEL_RESULT_PAGE_TEXT_Y, 0x120,
        DUEL_RESULT_PAGE_TEXT_HEIGHT
    );
    TextBox_BuildBlocking(object);
}

s32 Duel_CalcRankScoreChange(s32 arg0, s32 arg1)
{
    DuelRankScoreChangeEntry *p = &gDuel_awRankScoreChange[arg0][0];

    while (1) {
        if (arg1 < p->threshold) {
            return p->score_change;
        }
        p++;
    }
}

/* Initializes result-message selectors at +0x34; the winner's signed
   end-reason adjustment selects the middle variant, not a rank letter.
   Both side scores at +0x2C start at DUEL_RANK_SCORE_INITIAL, then receive
   the end-reason and threshold-rule adjustments. Raw statistics are also
   copied into the separate gText_StagedValues[0].rank_rows[stat][side] display
   table. */
/* The results page that shows the selected text. */
#ifdef VERSION_EUROPE
#define DUEL_RESULT_SELECTOR_PAGE 0
#else
#define DUEL_RESULT_SELECTOR_PAGE 1
#endif

void Duel_CalcRankScore(void) {
    DuelResultDisplayState *p;
    DuelSideState *e;
    s32 i;
    s32 v;
    s32 *q;

    p = gDuel_pResultDisplayState;
    e = gDuel_aSideStates;
    q = &gText_StagedValues[0].rank_rows[0][0];
#ifdef VERSION_EUROPE
    /* The European build shows the selected text on the first page and
       0x44 on the second. */
    p->page_text_ids[0] = DUEL_RESULT_TEXT_SELECTOR_DEFAULT;
    p->page_text_ids[1] = 0x44;
#else
    p->page_text_ids[0] = 0x44;
    p->page_text_ids[1] = DUEL_RESULT_TEXT_SELECTOR_DEFAULT;
#endif
    p->page_text_ids[2] = 0x45;
    if (gDuel_aSideStates[gDuel_bWinnerSide].rank.result_adjustment ==
        DUEL_RANK_ADJUST_EXODIA_WIN) {
        p->page_text_ids[DUEL_RESULT_SELECTOR_PAGE] =
            DUEL_RESULT_TEXT_SELECTOR_EXODIA;
    }
    if (gDuel_aSideStates[gDuel_bWinnerSide].rank.result_adjustment ==
        DUEL_RANK_ADJUST_DECK_OUT_WIN) {
        p->page_text_ids[DUEL_RESULT_SELECTOR_PAGE] =
            DUEL_RESULT_TEXT_SELECTOR_DECK_OUT;
    }

    p->side_scores[1] = DUEL_RANK_SCORE_INITIAL;
    p->side_scores[0] = DUEL_RANK_SCORE_INITIAL;
    for (i = 0; i < DUEL_SIDE_COUNT; i++, e++, q++) {
        p->side_scores[i] += e->rank.result_adjustment;
        v = e->deck_draw_cursor; q[0 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_CARDS_USED, v);
        v = e->life_points.signed_value; q[1 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_REMAINING_LP, v);
        q[2 * DUEL_SIDE_COUNT] = e->field_0E;
        q[3 * DUEL_SIDE_COUNT] = e->rank.field_0B;
        v = e->rank.effective_attacks; q[4 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_EFFECTIVE_ATTACKS, v);
        q[5 * DUEL_SIDE_COUNT] = e->field_10;
        q[6 * DUEL_SIDE_COUNT] = e->rank.field_0C;
        v = e->rank.defensive_wins; q[7 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_DEFENSIVE_WINS, v);
        q[8 * DUEL_SIDE_COUNT] = e->rank.field_07;
        v = e->rank.face_down_plays; q[9 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_FACE_DOWN_PLAYS, v);
        v = e->rank.fusions_initiated; q[10 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_INITIATE_FUSION, v);
        v = e->rank.equips_used; q[11 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_EQUIP_MAGIC, v);
        q[12 * DUEL_SIDE_COUNT] = e->rank.field_0A;
        v = e->rank.pure_magic_used; q[13 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_PURE_MAGIC, v);
        v = e->rank.traps_triggered; q[14 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_TRAPS_TRIGGERED, v);
        v = e->rank.turns_taken; q[15 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_TURNS, v);
    }
}

s32 Duel_SelectCardDrop(s32 pool_index)
{
    DuelDropTable *table = &gDuel_awSaPowCardDrops[pool_index];
    s32 threshold = (rand() & (DUEL_DROP_WEIGHT_TOTAL - 1)) + 1;
    s32 sum = 0;
    s32 i;

    for (i = 0; i < CARD_COUNT; i++) {
        sum += table->weights[i];
        if (sum >= threshold)
            return i + 1;
    }
    return 0;
}

void Duel_AwardCard(s32 card_id)
{
    s32 i;
    u8 *base = (u8 *)gDuel_awPlayerDeck;
    u8 *quantity = base + (card_id + 0x4F);
    u16 *destination = (u16 *)(base + 0x5BC);
    u16 *entry;

    (*quantity)++;
    if (*quantity >= CARD_CHEST_QUANTITY_MAX + 1)
        *quantity = CARD_CHEST_QUANTITY_MAX;
    i = DUEL_RECENT_CARD_DROP_COUNT - 2;
    entry = destination + (DUEL_RECENT_CARD_DROP_COUNT - 2);
    do {
        entry[1] = entry[0];
        entry--;
        i--;
    } while (i >= 0);
    *destination = card_id;
}

/* Regional values: the y of the starchip prize sprites (192 -> 210) and of
 * the TEC and rank sprites (16 -> 40). The European build
 * (src/game/european/) defines its own. */
#ifndef DUEL_RESULT_STARCHIP_Y
#define DUEL_RESULT_STARCHIP_Y 192
#endif
#ifndef DUEL_RESULT_RANK_Y
#define DUEL_RESULT_RANK_Y 16
#endif

/* The confirm button (Cross). The Japanese release swaps Cross and Circle, so a
 * regional build names its own. */
#ifndef DUEL_RESULT_REWARDS_CONFIRM_BUTTON
#define DUEL_RESULT_REWARDS_CONFIRM_BUTTON 0x40
#endif

void DuelScene_UpdateResultRewards(void)
{
    DisplayObject *object;
    s32 score;
    s32 sound;
    s32 count;
    s32 offset;
    s32 x;
    s16 dropped_card;
    u16 value;
    s8 page;
    u8 opponent;

    gViewState_Current.angle = (u16)gViewState_Current.angle + 2;
    ViewState_ApplyOrbit();
    if (!(gDuel_wSceneStateFlags & 0x8000)) {
        gDuel_wSceneStateFlags |= 0x8000;
        Fade_StartOutKeepOverlay();
        Fade_SetTargetLevel(128, 2);
        gDuel_pResultDisplayState = &gDuel_awRitualData;
        gGraphics_sViewportY = 0;
        gGraphics_sViewportX = 0;
        gText_BranchSelectorA = 0;
        gText_BranchSelectorB = 0;
        gText_abColorSlots[0] = 4;
        gText_abColorSlots[1] = 4;
        gText_abColorSlots[gDuel_bWinnerSide] = 0;
        sound = 0x72E1;
        if (gDuel_bWinnerSide) {
            sound = 0x72F1;
            opponent = (u8)gDuel_bOpponentID;
            gText_BranchSelectorB = 1;
            gText_StagedStringId = (s8)opponent - 31960;
        }
        SD_BGMPlay(sound);
        if (gDuel_bSide0ControllerSelector[0] < 0 && gDuel_bOpponentID < 0) {
            gText_BranchSelectorA = 1;
            gText_BranchSelectorB += 2;
        }
        gDuel_pResultDisplayState->is_tec_rank = 0;
        Duel_CalcRankScore();
        score = gDuel_pResultDisplayState->side_scores[gDuel_bWinnerSide];
        if (score < 50) {
            gDuel_pResultDisplayState->is_tec_rank = 1;
            if (score < 0)
                score = 0;
            score = 99 - score;
        }
        if (score >= 100)
            score = 99;
        score -= 50;
        gDuel_pResultDisplayState->rank_tier = score / 10;
        gDuel_pResultDisplayState->page_index = 0;
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPosition(object, 32, 16, 3, 1, 2, 11, 524);
        DisplayObject_SelectOrderingTable1(object);
        object->flags |= 0x28;
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPosition(object, 288, 16, 3, 1, 0, 11, 524);
        DisplayObject_SelectOrderingTable1(object);
        count = 9;
        object->flags |= 0x28;
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 8, 0, 4, 0, 16, 8, gDisplay_abResourceBank0);
        DisplayObject_SelectOrderingTable1(object);
        DisplayObject_SetDepthOffset(object, -1);
        object->flags |= 0x20;
        gDuel_pResultDisplayState->root = object;
        do {
            gDuel_pResultDisplayState->children[count] = 0;
            count--;
        } while (count >= 0);
        gDuel_wSelectedCardID = 0;
        if (gDuel_bSide0ControllerSelector[0] < 0 && gDuel_bOpponentID >= 0) {
            if (gDuel_bWinnerSide)
                goto side_result;
            gDuel_pResultDisplayState->starchip_prize = gDuel_pResultDisplayState->rank_tier + 1;
            score = 2 * (gDuel_pResultDisplayState->is_tec_rank != 0);
            if (gDuel_pResultDisplayState->rank_tier < 3)
                score = 1;
            dropped_card = Duel_SelectCardDrop(score);
            count = 0;
            gDuel_wSelectedCardID = dropped_card;
            gDuel_pResultDisplayState->dropped_card_id = dropped_card;
            gDuel_DroppedCardTextValue[0] = dropped_card;
            offset = 8;
            if (gDuel_pResultDisplayState->starchip_prize) {
                x = 160;
                do {
                    count++;
                    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
                    DisplayObject_ConfigureSpriteAtPosition(object, x, DUEL_RESULT_STARCHIP_Y, 3, 4, 0, 11, 524);
                    DisplayObject_SelectOrderingTable1(object);
                    x += 20;
                    object->flags |= 0x20;
                    *(DisplayObject **)((u8 *)gDuel_pResultDisplayState + offset + 4) = object;
                    offset += 4;
                } while (count < gDuel_pResultDisplayState->starchip_prize);
            }
        }
side_result:
        if (gDuel_bSide0ControllerSelector[gDuel_bWinnerSide] < 0) {
            object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
            DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, DUEL_RESULT_RANK_Y, 0, 5, gDuel_pResultDisplayState->is_tec_rank,
                16, 8, gDisplay_abResourceBank0);
            DisplayObject_SelectOrderingTable1(object);
            DisplayObject_SetDepthOffset(object, -2);
            object->flags |= 0x20;
            gDuel_pResultDisplayState->children[0] = object;
            object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
            DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, DUEL_RESULT_RANK_Y, 0, 6, gDuel_pResultDisplayState->rank_tier,
                16, 8, gDisplay_abResourceBank0);
            DisplayObject_SelectOrderingTable1(object);
            DisplayObject_SetDepthOffset(object, -1);
            object->flags |= 0x20;
            gDuel_pResultDisplayState->children[1] = object;
        }
        goto show_page;
    }
    if (gDuel_wSceneStateFlags & 0x4000) {
        if (!(((FadeTransitionState *)D_800E9EC8_arr)->flags & 0x80)) {
            if (!(gDuel_wSceneStateFlags & 0x2000)) {
                gDuel_wSceneStateFlags |= 0x2000;
                Fade_StartOut();
                ((FadeTransitionState *)D_800E9EC8_arr)->level = 255;
                Fade_FillBandLevels(255);
            } else {
                /* Only the null test goes through `save`; the updates below
                   index gDuel_pSide0SaveData again each time, as retail reloads the
                   window pointer (106 differences through `save`). */
                SaveDataState *save = gDuel_pSide0SaveData[gDuel_bWinnerSide];
                gDuel_wOutcomeFlags |= 0x2000;
                if (save) {
                    if (gDuel_bSide0ControllerSelector[0] < 0 && gDuel_bOpponentID >= 0) {
                        gDuel_pSide0SaveData[0]->starchips +=
                            gDuel_pResultDisplayState->starchip_prize;
                        if (gDuel_pSide0SaveData[0]->starchips > 999999)
                            gDuel_pSide0SaveData[0]->starchips = 999999;
                        Duel_AwardCard(gDuel_pResultDisplayState->dropped_card_id);
                    } else {
                        value = gDuel_pSide0SaveData[gDuel_bWinnerSide]->duel_wins + 1;
                        gDuel_pSide0SaveData[gDuel_bWinnerSide]->duel_wins = value;
                        if (value >= 10000)
                            gDuel_pSide0SaveData[gDuel_bWinnerSide]->duel_wins = 9999;
                        value = gDuel_pSide0SaveData[gDuel_bWinnerSide ^ 1]
                                    ->duel_losses + 1;
                        gDuel_pSide0SaveData[gDuel_bWinnerSide ^ 1]->duel_losses = value;
                        if (value >= 10000)
                            gDuel_pSide0SaveData[gDuel_bWinnerSide ^ 1]
                                ->duel_losses = 9999;
                    }
                }
            }
        }
    } else if (gInput_wPad1Repeat & 0xA000) {
        gDuel_pResultDisplayState->page_index++;
        if (gInput_wPad1Repeat & 0x8000) {
            page = gDuel_pResultDisplayState->page_index - 2;
            gDuel_pResultDisplayState->page_index = page;
            if (page < 0)
                gDuel_pResultDisplayState->page_index = 2;
        }
        if ((s8)gDuel_pResultDisplayState->page_index >= 3)
            gDuel_pResultDisplayState->page_index = 0;
        SD_SEPlayFull(6);
show_page:
        Duel_ShowResultPage((s8)gDuel_pResultDisplayState->page_index);
    } else if (gInput_wPad1Pressed & DUEL_RESULT_REWARDS_CONFIRM_BUTTON) {
        gDuel_wSceneStateFlags |= 0x4000;
        Fade_SetTargetLevel(0, 6);
        SD_SEPlayFull(0x30);
    }
}
