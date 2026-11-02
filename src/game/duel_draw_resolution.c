/*
 * Draw resolution and the Exodia result: Duel_UpdateDrawCardSlide eases a
 * drawn card into place, Duel_HasAllExodiaPieces and
 * DuelScene_UpdateDrawResolution resolve the drawn card, DuelScene_UpdateExodiaResult
 * stages the five-piece presentation, and DuelCard_CaptureRoundedTexture reads a
 * drawn card back into the staging buffer. One object in every build.
 */
#define FUNC_80018004_AMBIENT_POSITION_ARGS
#define gDuel_bEffectRequestStatus_IN_DATA
#define D_8009B369_IN_DATA
#define MAIN_MODE_STATE_NEXT_IN_DATA
#define MAIN_MODE_STATE_ACTIVE_IN_DATA
#define ANIMATED_BATTLE_MODEL_PROPERTIES_AS_HALFWORDS
#define D_8009B214_AS_BYTE_POINTER
#define D_8009B21C_AS_BYTE_POINTER
#define D_8009B269_AS_SCALAR_DATA
#define D_8009B26C_AS_SCALAR_DATA
#define GRAPHICS_ACTIVE_BUFFER_IN_DATA
#include "../types.h"
#include "duel_card_staging.h"
#include "duel_draw_resolution.h"
#include "duel_card.h"
#include "duel_grid.h"
#include "duel_side_state.h"
#include "duel_scene_state.h"
#include "card_constants.h"
#include "duel_hand.h"
#include "duel_card_layout.h"
#include "duel_deck_card.h"
#include "duel_rank.h"
#include "sound.h"
#include "../unmatched.h"
#include "duel_field_card_render_data.h"
#include "fade_state_bytes.h"
#include "duel_session_controls.h"
#include "duel_card_display_objects.h"
#include "duel_update_draw_card_slide.h"
#include "display_object_helpers.h"
#include "duel_effect_request.h"
#include "duel_card_pick_cursor.h"
#include "duel_selection_layout.h"
#include "duel_scene_card_placement.h"
#include "duel_presentation_controls.h"
#include "duel_scene_exodia_result.h"
#include "duel_screen_tables.h"
#include "rand_get_interval.h"
#include "fade.h"
#include "display_object_motion.h"
#include "display_object_work_slots.h"
#include "display_object_core.h"
#include "duel_init_scene.h"
#include "duel_effect.h"
#include "duel_effect_allocate_request.h"
#include "sound_output.h"
#include "model_scene_states.h"
#include "main_modes.h"
#include "model.h"
#include "../psyq/rand.h"
#include "main_mode_state.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "color_constants.h"
#include "display_object.h"
#include "duel_display.h"
#include "duel_card_capture_rounded_texture.h"
#include "graphics_frame.h"
#include "image_transfer_rects.h"
#include "display_object_interpolation.h"
#include "display_object_lifecycle.h"

#define DUEL_EFFECT_REQUEST_FIELD_1D_OFFSET \
    ((u32)&((DuelEffectRequest *)0)->progress)
#define DISPLAY_OBJECT_VIEW(object) ((DisplayObject *)(object))

#define DUEL_CARD_READBACK_HALFWORDS(buffer) ((u16 *)(buffer))
#define DUEL_CARD_READBACK_WORDS(buffer) ((u32 *)(buffer))

/* Regional value: the y Duel_CreateHandCardDisplayObject places the drawn card at (0x92 ->
 * 0xA2). The European build (src/game/european/) defines its own. */
#ifndef DUEL_DRAW_RESOLUTION_CARD_Y
#define DUEL_DRAW_RESOLUTION_CARD_Y 0x92
#endif

void Duel_UpdateDrawCardSlide(DisplayObject *o)
{
    if (!DisplayObject_MarkInitialized((DisplayObjectLifecycle *)o)) {
        o->field_2C.h.field_2C = 0x400 / o->field_60;
        o->position.h.field_28 = o->field_30.h.field_30 - 0x140;
        DisplayObject_SavePosition((DisplayObjectSnapshot *)o);
        o->field_60 = -0x400;
    }
    Widget_SlideSine((DisplayObjectPosition *)o, (s16)o->position.h.field_28,
                  (s16)o->field_30.h.field_32, o->field_60);
    o->field_60 += o->field_2C.h.field_2C;
    if (o->field_60 >= 0) {
        o->update = 0;
        o->field_6C = 0;
        o->field_30.h.field_30 = (s16)o->position.h.field_28;
    }
}

s32 Duel_HasAllExodiaPieces(void) {
    s16 hand[HAND_SIZE];
    s32 i;
    s32 card_id;

    for (i = 0; i < HAND_SIZE; i++) {
        hand[i] = gDuel_pActiveSideState->hand[i];
    }

    for (card_id = EXODIA_FIRST_CARD_ID;
         card_id < EXODIA_CARD_ID_END;
         card_id++) {
        for (i = 0; i < HAND_SIZE; i++) {
            s16 index = hand[i];
            if (index >= 0) {
                s16 id = ((DuelCardStagingDeckView *)gDuel_abCardStagingBuffer)->cards[index].id;
                if (id == card_id) {
                    hand[i] = -1;
                    goto found;
                }
            }
        }
        return 0;
    found:;
    }

    return 1;
}

void DuelScene_UpdateDrawResolution(void) {
    DisplayObject *p;
    DuelSideState *c;
    DuelCardReplayRecordBlock *g;
    u8 *base;
    s32 i;
    s32 k;
    s32 v;
    s32 t;
    s32 b;
    s32 a;
    s32 n;
    s32 off;
    s32 y;

    v = gDuel_wSceneStateFlags;
    if ((v & DUEL_SCENE_FLAG_INITIALIZED) == 0) {
        gDuel_wSceneStateFlags = v | DUEL_SCENE_FLAG_INITIALIZED;
        gDuel_bDrawStepCountdown = 1;
    }

    if ((gDuel_wSceneStateFlags & 0x4000) == 0) {
        t = gDuel_bDrawStepCountdown - 1;
        gDuel_bDrawStepCountdown = t;
        if ((s8)t > 0) {
            return;
        }
        gDuel_bDrawStepCountdown = 8;
        b = gDuel_bPendingDrawCount - 1;
        c = gDuel_pActiveSideState;
        gDuel_bPendingDrawCount = b;
        if (c->deck_draw_cursor >= DECK_SIZE) {
            gDuel_bWinnerSide = gDuel_bActiveSide ^ 1;
            gDuel_aSideStates[gDuel_bWinnerSide].rank.result_adjustment =
                DUEL_RANK_ADJUST_DECK_OUT_WIN;
            gDuel_wSceneStateFlags = 0xC;
            return;
        }
        i = HAND_SIZE - 1 - (s8)b;
        a = gDuel_abHandCardRecordIndices[i + gDuel_bActiveSide * HAND_SIZE];
        if ((a & 0x80) != 0) {
            k = (a & 0x7F) + DUEL_CARD_SIDE_RECORD_COUNT;
        } else {
            k = a;
        }
        Duel_SetupCardRecord(k, gDuel_pActiveSideState->deck_draw_cursor);
        p = (DisplayObject *)Duel_CreateHandCardDisplayObject(
            &gDuel_aCardRecords[k],
            i * 60 + 0x14E,
            DUEL_DRAW_RESOLUTION_CARD_Y
        );
        p->field_6C = 1;
        p->field_60 = 0xC;
        p->update = (DisplayObjectCallback)Duel_UpdateDrawCardSlide;
        gDuelHand_aSlots[i].object = p;
        base = gDuel_abCardStagingBuffer;
        g = (DuelCardReplayRecordBlock *)(
            (u8 *)&((DuelCardRecord *)base)[p->field_6A] +
            DUEL_CARD_STAGING_REPLAY_BASE_OFFSET);
        y = ((DuelDeckCardRecord *)g->record.data)->deck_index;
        gDuel_pActiveSideState->hand[i] = y;
        n = (u8)gDuel_pActiveSideState->deck_draw_cursor;
        gDuel_pActiveSideState->deck_draw_cursor = n + 1;
        if (*(s8 *)&gDuel_bPendingDrawCount == 0) {
            gDuel_wSceneStateFlags = gDuel_wSceneStateFlags | 0x4000;
        }
        SD_SEPlayFull(0xA);
    } else {
        if (DisplayObject_FindAllocatedByTag(1) == 0) {
            gDuel_wSceneStateFlags = 4;
            if (Duel_HasAllExodiaPieces() != 0) {
                gDuel_wSceneStateFlags = 0xE;
            }
        }
    }
}

/*
 * Stages the five-card Exodia presentation, sparkle phases, centre burst, and
 * result handoff. The actual request owner with integer buffer-word views, typed
 * DisplayObject cursor, and cross-path reuse of `side` preserve the retail
 * scheduling and register allocation. The first position store keeps a signed
 * address-of-member view so GCC materializes -0x40 like retail. The two
 * request-completion tests read the actual DuelEffectRequest::progress byte.
 */
extern s32 D_800E9F04[];

void DuelScene_UpdateExodiaResult(void)
{
    u16 flags;
    DisplayObject *obj;
    u8 *pose;
    u8 *rec;
    DuelEffectRequest *fx;
    s32 i;
    s32 anim;
    s32 n;
    s32 r;
    s32 t14;
    u32 sum;
    u8 *cards;
    u8 *poses;
    DisplayObject *G32 *objs;
    s32 fnv;
    DuelCardReplayRecordBlock *g;
    s32 py;
    s32 t;
    s32 k;
    DisplayObject *G32 *slot;
    s8 side;
    DuelSideState *other;
    DisplayObject *d;

    flags = gDuel_wSceneStateFlags;
    if ((flags & DUEL_SCENE_FLAG_INITIALIZED) == 0) {
        i = 0;
        cards = gDuel_abCardStagingBuffer;
        poses = (u8 *)&gDuel_ExodiaCardPoses;
        r = (s32)DisplayObject_UpdateCosineSlide;
        fnv = r;
        objs = gDisplayObject_apWorkSlots;
        gDuel_wSceneStateFlags = flags | DUEL_SCENE_FLAG_INITIALIZED;
        obj = DISPLAY_OBJECT_VIEW(gDuel_pTerrainIcon);
        side = gDuel_bActiveSide;
        gDuel_pActiveSelectionRecord =
            (DuelSelectionRecord *)&gDuel_abSelectionSides[
                (u8)side * DUEL_SELECTION_SIDE_SIZE
            ];
        *(s16 *)&obj->position.h.field_28 = -0x40;
        obj->field_2C.h.field_2C = 0x10;
        obj->field_6C = 1;
        obj->update = (DisplayObjectCallback)fnv;
        obj->position.h.field_2A = obj->field_30.h.field_32;
        obj = DISPLAY_OBJECT_VIEW(gDuel_pLifePointsPanel);
        obj->position.h.field_28 = 0x180;
        obj->field_2C.h.field_2C = 0x10;
        obj->field_6C = 1;
        obj->update = (DisplayObjectCallback)fnv;
        obj->position.h.field_2A = obj->field_30.h.field_32;
        rec = (u8 *)gDuelHand_aSlots;
next_obj:
        obj = *(DisplayObject *G32 *)rec;
        /* Keep the typed record offset left of the staging base. */
        g = (DuelCardReplayRecordBlock *)(
            (u32)&((DuelCardRecord *)0)[obj->field_6A] +
            (u32)cards + DUEL_CARD_STAGING_REPLAY_BASE_OFFSET
        );
        anim = g->record.card_id - 0x11;
        pose = (u8 *)(anim * 3 + (u32)poses);
        obj->position.h.field_28 = pose[1] - 0x1A;
        py = pose[2];
        obj->field_2C.h.field_2C = 0xB4;
        obj->field_6C = 1;
        obj->update = (DisplayObjectCallback)fnv;
        obj->position.h.field_2A = py - 0x1E;
        objs[pose[0]] = obj;
        *(DisplayObject *G32 *)rec = 0;
        i++;
        rec += sizeof(DuelHandSlot);
        if (i < DISPLAY_OBJECT_WORK_SLOT_COUNT) {
            goto next_obj;
        }
        D_800E9F04[0] = 0;
        DisplayObject_ReleaseIfPresent(
            gDuel_pActiveSelectionRecord->cursor_object);
        gDuel_wDisplayTransitionFlags = 8;
        gDuel_wStepCounter = 0;
        gDuel_bPresentationCursor = 0;
        gDuelEffect_pTrackedRequest = 0;
        gDuel_wSceneStateFlags = gDuel_wSceneStateFlags | 0x4000;
        Fade_StartOutKeepOverlay();
        gFade_bStep[0] = 2;
        SD_BGMFadeOutWithStep(2);
        return;
    }
    if (flags & 0x4000) {
        if (flags & 0x2000) {
            if ((gDuel_bEffectRequestStatus & 1) != 0) {
                return;
            }
            gDuel_wSceneStateFlags = flags & 0xBFFF;
            gDuel_bPresentationCursor = 0;
            gDuel_wStepCounter = 0;
            return;
        }
        if (gDuelEffect_pTrackedRequest != 0) {
            if (gDuelEffect_pTrackedRequest->progress == 0) {
                return;
            }
            SD_SEPlayFull(0x1D);
            if (DisplayObject_FindAllocatedByTag(1) == 0) {
                gDuel_wSceneStateFlags = gDuel_wSceneStateFlags | 0x2000;
                return;
            }
        }
        fx = DuelEffect_AllocateRequest(0x13);
        gDuelEffect_pTrackedRequest = fx;
        fx->field_00 = (s16)((rand() & 0xFF) + 0x20);
        r = Rand_GetInterval(0xB0);
        n = gDuel_bPresentationCursor;
        t14 = (s32)fx->buffer;
        fx->field_02 = (s16)(r + 0x20);
        fx->buffer = (void *)(t14 + ((n & 3) << 13));
        gDuel_bPresentationCursor = n + 1;
        return;
    }
    if (flags & 0x2000) {
        t = gDuel_wStepCounter - 1;
        gDuel_wStepCounter = t;
        if ((s16)t > 0) {
            return;
        }
        if ((s8)gDuel_bPresentationCursor >= DISPLAY_OBJECT_WORK_SLOT_COUNT) {
            gDuel_wSceneStateFlags = (flags & 0xDFFF) | 0x1000;
            fx = DuelEffect_AllocateRequest(0x13);
            fx->field_00 = 0xA0;
            fx->field_02 = 0x78;
            gDuelEffect_pTrackedRequest = fx;
            fx->buffer = (void *)((s32)fx->buffer + 0x8000);
            return;
        }
        gDuel_wStepCounter = 4;
        fx = DuelEffect_AllocateRequest(0);
        k = (s8)gDuel_bPresentationCursor;
        slot = &gDisplayObject_apWorkSlots[k];
        d = *slot;
        fx->field_00 = (s16)(d->field_30.h.field_30 + 0x1A);
        d = *slot;
        fx->field_02 = (s16)(d->field_30.h.field_32 + 0x1E);
        sum = (k << 12) + 0xA000;
        anim = (s32)fx->buffer + sum;
        fx->buffer = (void *)anim;
        fx->phase = 9;
        SD_SEPlayFull(0x17);
        gDuel_bPresentationCursor = gDuel_bPresentationCursor + 1;
        return;
    }
    if (flags & 0x1000) {
        if (gDuelEffect_pTrackedRequest->progress == 0) {
            return;
        }
        gDuel_wSceneStateFlags = flags & 0xEFFF;
        SD_SEPlayFull(0x1D);
        DuelEffect_CreateRequest(0x18);
        SD_CdSoundPlay(0x8021);
        return;
    }
    Fade_CutToWhite();
    gMain_aAnimatedBattleModels[0] = MODEL_SPECIAL_BATTLE_ID;
    side = gDuel_bActiveSide;
    gDuel_bWinnerSide = side;
    gDuel_aSideStates[(u8)side].rank.result_adjustment = 0x28;
    other = &gDuel_aSideStates[gDuel_bActiveSide ^ 1];
    other->life_points.unsigned_value = 0;
    other->displayed_life_points = 0;
    SD_BGMPreload(0x7310);
    Model_SetPresentationBgmCommand(0x7310);
    gDuel_bResumeFromAnimatedBattle = 1;
    gMain_bReturnMode = 3;
    gMain_bActiveModeState = 1;
}

void DuelCard_CaptureRoundedTexture(DisplayObject *o)
{
    u8 *buf;
    u16 *p;
    s32 xoff;

    while (IsIdleGPU(10) != 0) {
        ;
    }

    xoff = 0;
    if (gGraphics_bActiveBuffer == 0) {
        xoff = 0x140;
    }

    buf = gDuel_abCardStagingBuffer;
    gGraphics_aImageTransferRects[0].x = o->field_30.h.field_30 + xoff;
    gGraphics_aImageTransferRects[0].y = o->field_30.h.field_32;
    gGraphics_aImageTransferRects[0].w = DUEL_CARD_READBACK_WIDTH_WORDS;
    gGraphics_aImageTransferRects[0].h = DUEL_CARD_READBACK_HEIGHT;
    StoreImage2(&gGraphics_aImageTransferRects[0], DUEL_CARD_READBACK_WORDS(buf));

    p = DUEL_CARD_READBACK_HALFWORDS(buf);
    xoff = DUEL_CARD_READBACK_WORD_COUNT;
    do {
        *p = *p | COLOR_BGR555_STP_MASK;
        p++;
        xoff--;
    } while (xoff != 0);

    /* The loop above set the STP bit on every pixel; these clear twelve
       corner pixels of the 140x196 readback back to 0 -- (0, 0), (0, 1),
       (1, 0) and the matching three at each other corner -- which rounds
       the card's corners. (0, 0) is the store through gDuel_abCardStagingBuffer below. */
    p = DUEL_CARD_READBACK_HALFWORDS(gDuel_abCardStagingBuffer);
    p[(DUEL_CARD_READBACK_HEIGHT - 1) * DUEL_CARD_READBACK_WIDTH_WORDS + 138] = 0;
    p[(DUEL_CARD_READBACK_HEIGHT - 1) * DUEL_CARD_READBACK_WIDTH_WORDS + 139] = 0;
    p[(DUEL_CARD_READBACK_HEIGHT - 1) * DUEL_CARD_READBACK_WIDTH_WORDS + 1] = 0;
    p[(DUEL_CARD_READBACK_HEIGHT - 1) * DUEL_CARD_READBACK_WIDTH_WORDS + 0] = 0;
    p[(DUEL_CARD_READBACK_HEIGHT - 2) * DUEL_CARD_READBACK_WIDTH_WORDS + 139] = 0;
    p[(DUEL_CARD_READBACK_HEIGHT - 2) * DUEL_CARD_READBACK_WIDTH_WORDS + 0] = 0;
    p[1 * DUEL_CARD_READBACK_WIDTH_WORDS + 139] = 0;
    p[1 * DUEL_CARD_READBACK_WIDTH_WORDS + 0] = 0;
    p[0 * DUEL_CARD_READBACK_WIDTH_WORDS + 138] = 0;
    p[0 * DUEL_CARD_READBACK_WIDTH_WORDS + 139] = 0;
    p[0 * DUEL_CARD_READBACK_WIDTH_WORDS + 1] = 0;

    DUEL_CARD_READBACK_HALFWORDS(gDuel_abCardStagingBuffer)[0] = 0;
    gGraphics_aImageTransferRects[0].x = 0x140;
    gGraphics_aImageTransferRects[0].y = 0x100;
    gGraphics_aImageTransferRects[0].w = DUEL_CARD_READBACK_WIDTH_WORDS;
    gGraphics_aImageTransferRects[0].h = DUEL_CARD_READBACK_HEIGHT;
    LoadImage2(&gGraphics_aImageTransferRects[0], DUEL_CARD_READBACK_WORDS(gDuel_abCardStagingBuffer));
}
