

#define D_8009B369_IN_DATA
#define D_8009B204_UNSIGNED
#define DUEL_TERRAIN_SCALAR_IN_DATA
#include "../types.h"
#include "duel_scene_state.h"
#include "duel_shuffle_both_decks.h"
#include "duel_card_and_status_draw.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_deck_card.h"
#include "duel_display.h"
#include "duel_card_display_state.h"
#include "save_data.h"
#include "save_pair_workspace.h"
#include "duel_grid.h"
#include "duel_card_pick_cursor.h"
#include "duel_selection_layout.h"
#include "duel_hand.h"
#include "main_frame.h"
#include "duel_side_state.h"
#include "duel_shuffle_deck.h"
#include "duel_check_quit_input.h"
#include "display_object_core.h"
#include "display_object_layout.h"
#include "duel_package.h"
#include "file_transfer.h"
#include "duel_load_package_stage.h"
#include "display_object_helpers.h"
#include "view_state.h"
#include "main_services.h"
#include "duel_effect_resource_record.h"
#include "sound_pending_entries.h"
#include "sound_voice_selection.h"
#include "duel_action_lock.h"
#include "duel_draw_card_output_position.h"
#include "display_object_set_resource_variant_from_sign.h"
#include "../unmatched.h"
#include "duel_session_controls.h"
#include "duel_init_scene.h"
#include "duel_init_model_scene.h"
#include "duel_effect_object_pool.h"
#include "card_preview_objects.h"
#include "text_render_state.h"
#include "duel_init_ground_projection_reference.h"
#include "trig_constants.h"
#include "duel_terrain_boost.h"
#include "duel_view_tween.h"
#include "ai_opponent_data.h"
#include "view_state_current.h"
#include "duel_card_object_helpers.h"
#include "duel_card_layout.h"
#include "duel_card_display_objects.h"

/* Initializes the duel scene, then selects and shuffles both deck buffers. */

/* The terrain package's first sector and length. The Japanese WA.MRG gives
   each terrain 0xEF sectors from 0x16B5 (see duel_load_terrain_package.c),
   so a regional build supplies its own. */
#ifndef DUEL_INIT_TERRAIN_SECTOR
#define DUEL_INIT_TERRAIN_SECTOR(value) \
    ((((value) * 15) * 4 - (value)) * 4 - (value)) + \
        DUEL_TERRAIN_PACKAGE_FIRST_SECTOR
#define DUEL_INIT_TERRAIN_SECTOR_COUNT DUEL_TERRAIN_PACKAGE_SECTOR_COUNT
#endif
/* Likewise the duelist data's first sector (duel_result_runtime.c's
   DUEL_RESULT_DUELIST_DATA_FIRST_SECTOR is the same value). */
#ifndef DUEL_INIT_DUELIST_DATA_FIRST_SECTOR
#define DUEL_INIT_DUELIST_DATA_FIRST_SECTOR DUELIST_DATA_FIRST_SECTOR
#endif
#ifndef DUEL_INIT_RESOURCE_FIELD_2C
#define DUEL_INIT_RESOURCE_FIELD_2C 0
#define DUEL_INIT_RESOURCE_FIELD_2E 0xFF
#endif

void Duel_InitScene(void)
{
    DisplayObject *obj;
    u8 *p;
    u8 *q;
    s32 value;
    s32 side;
    DuelEffectResourceRecord *pane;
    u8 *prev;
    s8 *pid;

    pid = &gDuel_bOpponentID;
    SD_ResetPendingEntries();
    SD_ReplaceValueLink(1);
    Main_AdvanceFrames(4);
    File_WaitForTransfers();
    value = gDuel_bTerrain;
    File_RequestAsyncTransfer(
        0, 0,
        DUEL_INIT_TERRAIN_SECTOR(value),
        DUEL_INIT_TERRAIN_SECTOR_COUNT, Duel_LoadPackageStage, 0, 0);
    File_WaitForTransfers();
    gDuel_bOutroWinnerSide = -1;
    gDuel_wSceneStateFlags = 11;
    gDuel_bQuitDialogState = 0;
    gDuel_wDisplayTransitionFlags = 0;
    gDuelCursor_PickFlags = 0;
    gDuel_wViewTweenFramesRemaining = 0;
    gDuel_wCardEffectFlags = 0;
    gDuel_wOutcomeFlags = 0;
    gDuel_bSceneSubStep = 0;
    if (gDuel_bResumeFromAnimatedBattle != 1) {
        if (gDuel_bOpponentID >= 0) {
            File_RequestAsyncTransfer(
                0, 0,
                gDuel_bOpponentID * (DUELIST_DATA_SECTOR_COUNT - 1) +
                    gDuel_bOpponentID + DUEL_INIT_DUELIST_DATA_FIRST_SECTOR,
                DUELIST_DATA_SECTOR_COUNT, 0, 0,
                (s32)gDuel_awOpponentDeckPool
            );
        }
        gDuel_bActiveSide = 0;
        gDuel_wSceneStateFlags = 1;
        Duel_ResetCardRecords();
        Duel_InitSelectionRecords();
        Duel_InitSideStates();
        gDuel_wOutcomeFlags |= 0x1000;
    }
    gDuel_pActiveSideState = &gDuel_aSideStates[gDuel_bActiveSide];
    Duel_InitGroundProjectionReference();
    gViewState_Current.angle =
        gDuel_bActiveSide * TRIG_ANGLE_HALF_TURN + TRIG_ANGLE_QUARTER_TURN;
    ViewState_ApplyOrbit();
    Duel_ClearHandSlots();
    pane = &gCardPreview_Resources[0];
    DuelEffect_ResetRequestPool();
    DuelEffect_ClearResourceObjectPointers(0);
    pane->src_x = 0;
    pane->src_y = 256;
    pane->field_2C = DUEL_INIT_RESOURCE_FIELD_2C;
    pane->field_2E = DUEL_INIT_RESOURCE_FIELD_2E;
    DuelEffect_ClearResourceObjectPointers(1);
    pane[1].src_x = 64;
    pane[1].src_y = 256;
    pane[1].field_2C = DUEL_INIT_RESOURCE_FIELD_2C;
    pane[1].field_2E = DUEL_INIT_RESOURCE_FIELD_2E - 1;
    Model_SetFogDrawFlags(0);
    Duel_InitModelScene();
    File_WaitForTransfers();
    gDuel_pActiveFieldGrid =
        &gDuel_abFieldGridCardRecordIndices[gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT];
    obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(obj, 12, 24, 4, 2, gDuel_bTerrain, 11, 732);
    DisplayObject_SelectOrderingTable1(obj);
    /* Both of this function's flag merges go through the member's address
       as s16 instead of obj->flags, and the spelling is load-bearing rather
       than untidy.

       Writing obj->flags lets GCC 2.8.1 schedule the sh into the delay slot
       of the following jal and drop the nops around it, which shortens the
       executable by sixteen bytes. *(u16 *)&obj->flags fails identically,
       because fold turns an address-of cast to the member's own type back
       into the member access; the s16 cast differs from the u16 member, so
       it stays a non-struct store. Each site was measured on its own:
       leaving only the second as member access still loses twelve bytes, and
       the first accounts for the other four. */
    *(s16 *)&obj->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    side = (u32)gDuel_bOpponentID >> 31;
    gDuel_pTerrainIcon = obj;
    obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(
        obj, 280, 32, 4, side, 0, 11, 748
    );
    DisplayObject_SelectOrderingTable1(obj);
    *(s16 *)&obj->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    if (gDuel_bActiveSide != 0) {
        *(u16 *)&obj->field_40.h.field_40 += 16;
    }
    gDuel_pLifePointsPanel = obj;
    obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);
    DisplayObject_SelectOrderingTable1(obj);
    DisplayObject_SetDepthOffset(obj, 1);
    obj->field_4C = (s32)Duel_DrawLifePointsAndDeckCounts;
    prev = (u8 *)gDuel_pLifePointsPanel;
    obj->field_50.word = (s32)prev;
    gMain_apFrameServiceCallbacks[3] = Duel_DrawFieldCards;
    if (gDuel_bResumeFromAnimatedBattle != 1) {
        p = 0;
        gDuel_pSide1SaveData = 0;
        gDuel_pSide0SaveData = 0;
        if (pid[-1] < 0) {
            if (gDuel_bOpponentID < 0) {
                gDuel_pSide0SaveData = gSaveData_aPairWorkspace;
                gDuel_pSide1SaveData = gSaveData_aPairWorkspace + TWO_PLAYER_SAVE_SLOT_STRIDE;
                Duel_ShuffleBothDecks(
                    (const u16 *)gSaveData_aPairWorkspace,
                    (const u16 *)(gSaveData_aPairWorkspace + TWO_PLAYER_SAVE_SLOT_STRIDE)
                );
                return;
            }
            p = (u8 *)gDuel_awPlayerDeck;
            gDuel_pSide0SaveData = p;
            if (gDuel_bOpponentID < DUEL_MASTER_K_OPPONENT_ID) {
                q = 0;
                goto shuffle;
            }
        }
        q = p;
    shuffle:
        Duel_ShuffleBothDecks((const u16 *)p, (const u16 *)q);
    }
}

void Duel_UpdateCardDisplayMarker(DuelCardDisplayObject *object)
{
    DuelCardRecord *card = &gDuel_aCardRecords[object->card_index];

    if ((*(s32 *)&card->terrain_modifier & 0xA0000000) != 0xA0000000) {
        return;
    }
    if (gDuel_pActiveSideState->card_view_mode != 0) {
        object->deck_number_marker = ((DuelDeckCardRecord *)card->data)->pre_shuffle_index + 1;
    }
    if (gDuel_pActiveSideState->card_view_mode < 0) {
        object->deck_number_marker = 0xFF;
    }
}

void Duel_UpdateCardDisplayState(DuelCardDisplayObject *object)
{
    DuelCardRecord *card = &gDuel_aCardRecords[object->card_index];
    u16 flags;

    if (!(card->flags & DUEL_CARD_FLAG_DISPLAY_MARKER)) {
        object->deck_number_marker = 0;
    }
    flags = object->flags & ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
    object->flags = flags;
    if (card->flags &
        (DUEL_CARD_FLAG_DEFENSE_POSITION | DUEL_CARD_FLAG_FACE_DOWN)) {
        object->flags = flags | DISPLAY_OBJECT_FLAG_CLIP_TEST;
        object->rotation_y = 0;
        if (card->flags & DUEL_CARD_FLAG_FACE_DOWN) {
            object->rotation_y = 0x80;
        }
        object->rotation_z = 0;
        if (card->flags & DUEL_CARD_FLAG_DEFENSE_POSITION) {
            object->rotation_z = 0xC0;
        }
    }
    object->color = DUEL_DISPLAY_COLOR_NORMAL;
    if (card->flags & DUEL_CARD_FLAG_USED_THIS_TURN) {
        object->color = DUEL_DISPLAY_COLOR_DIMMED;
    }
}

DuelCardDisplayObject *Duel_CreateCardDisplayObject(DuelCardRecord *arg0, s32 arg1, s32 arg2)
{
    DuelCardDisplayObject *p =
        DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);
    s32 *tbl;
    s32 k;

    k = arg0->card_id - 1;
    tbl = gDuel_adwCardStats;
    p->deck_number_marker = 0;
    p->card_type = (tbl[k] >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
    p->is_back_facing = 0;
    p->card_index =
        ((u32)arg0 - (u32)gDuel_aCardRecords) / DUEL_CARD_RECORD_SIZE;
    p->deck_index = ((u8 *)arg0->data)[2];
    p->out_x = arg1;
    p->out_y = arg2;
    p->attribute |= DISPLAY_OBJECT_ATTRIBUTE_8BPP;
    p->field_10.callback = DisplayObject_SetResourceVariantFromSign;
    DisplayObject_SelectOrderingTable1((DisplayObject *)p);
    p->field_4C = Duel_DrawCardAtOutputPosition;
    Duel_UpdateCardDisplayState(p);
    Duel_UpdateCardDisplayMarker(p);
    return p;
}

DuelCardDisplayObject *Duel_CreateHandCardDisplayObject(DuelCardRecord *card)
{
    DuelCardDisplayObject *result =
        /* The x/y words the caller left in $a1/$a2 pass through. */
        ((DuelCardDisplayObject *(*)())Duel_CreateCardDisplayObject)((u8 *)card);

    if (gDuel_pActiveSideState->card_view_mode != 0) {
        card->flags |= DUEL_CARD_FLAG_DISPLAY_MARKER;
        result->deck_number_marker =
            ((DuelDeckCardRecord *)card->data)->pre_shuffle_index + 1;
        if (gDuel_pActiveSideState->card_view_mode < 0) {
            result->deck_number_marker = 0xFF;
        }
    }
    return result;
}

/* Same gDuel_aCardRecords[] stat table (0x1C-byte stride) as Duel_UpdateCardDisplayState, but with
   the rotation_y/rotation_z bit mapping SWAPPED relative to it: here the face-down
   flag marks rotation_z, while the defense-position flag marks rotation_y.

   Clears the display flags' clip-test bit and rotation_z, then re-derives
   rotation_z and rotation_y from the selected gDuel_aCardRecords record's face-down and
   defense-position flags; always sets color to DUEL_DISPLAY_COLOR_NORMAL, or
   DUEL_DISPLAY_COLOR_DIMMED if DUEL_CARD_FLAG_USED_THIS_TURN is set; runs
   Duel_UpdateCardDisplayMarker, then clears deck_number_marker unless
   DUEL_CARD_FLAG_DISPLAY_MARKER is set. */
void Duel_ApplyCardObjectFlags(DuelCardDisplayObject *object) {
    u16 flags = object->flags;
    s32 type = object->card_index;
    DuelCardRecord *rec;

    object->rotation_z = 0;
    object->flags = flags & ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
    rec = &gDuel_aCardRecords[type];

    if (rec->flags & DUEL_CARD_FLAG_FACE_DOWN) {
        object->rotation_z = DUEL_CARD_DISPLAY_FACE_DOWN_VALUE;
    }
    object->rotation_y = 0;
    if (rec->flags & DUEL_CARD_FLAG_DEFENSE_POSITION) {
        object->rotation_y = DUEL_CARD_DISPLAY_DEFENSE_VALUE;
    }
    object->color = DUEL_DISPLAY_COLOR_NORMAL;
    if (rec->flags & DUEL_CARD_FLAG_USED_THIS_TURN) {
        object->color = DUEL_DISPLAY_COLOR_DIMMED;
    }
    Duel_UpdateCardDisplayMarker(object);
    if (!(rec->flags & DUEL_CARD_FLAG_DISPLAY_MARKER)) {
        object->deck_number_marker = 0;
    }
}

DisplayObject *Duel_CreateFieldCardPanel(int arg0, int arg1)
{
    DisplayObject *object =
        DisplayObject_AcquireSlot((s32)DisplayObject_FindFreeSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(object, arg0, arg1, 0, 1, 0, 0x1F, 0x100);
    DisplayObject_SelectOrderingTable1(object);
    DisplayObject_SetDepthOffset(object, -2);
    object->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    return object;
}

int Duel_GetCardEffectVariant(CardObject *object)
{
    int value = 1;
    switch (object->type) {
    case CARD_TYPE_MAGIC:
    case CARD_TYPE_EQUIP:
        value = 2;
        break;
    case CARD_TYPE_TRAP:
        value = 3;
        break;
    case CARD_TYPE_RITUAL:
        value = 4;
        break;
    }
    if (object->rotation_z != 0) value |= 0x80;
    return value;
}
