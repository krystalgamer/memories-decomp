#define D_8009B369_IN_DATA
#define D_8009B204_UNSIGNED
#define DUEL_TERRAIN_SCALAR_IN_DATA
#include "../types.h"
#include "duel_scene_state.h"
#include "duel_shuffle_both_decks.h"
#include "duel_draw_status_numbers.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_display.h"
#include "duel_card_display_state.h"
#include "save_data.h"
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
#include "func_80016778.h"
#include "../unmatched.h"
#include "func_800179F4.h"
#include "func_8001755C.h"
#include "duel_effect_object_pool.h"
#include "func_80029574.h"
#include "text_render_state.h"
#include "func_800178BC.h"
#include "trig_constants.h"
#include "duel_terrain_boost.h"
#include "func_80022D94.h"
#include "ai_opponent_data.h"

/* Initializes the duel scene, then selects and shuffles both deck buffers. */

void func_800179F4(void)
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
    func_8004763C();
    func_80047AD0(1);
    func_80012D84(4);
    File_WaitForTransfers();
    value = gDuel_bTerrain;
    File_RequestAsyncTransfer(
        0, 0,
        (((value * 15) * 4 - value) * 4 - value) +
            DUEL_TERRAIN_PACKAGE_FIRST_SECTOR,
        DUEL_TERRAIN_PACKAGE_SECTOR_COUNT, Duel_LoadPackageStage, 0, 0);
    File_WaitForTransfers();
    D_8009B238 = -1;
    gDuel_wSceneStateFlags = 11;
    gDuel_bQuitDialogState = 0;
    D_8009B162 = 0;
    D_8009B1D4 = 0;
    D_8009B204 = 0;
    gDuel_wCardEffectFlags = 0;
    D_8009B16C = 0;
    D_8009B174 = 0;
    if (D_8009B369 != 1) {
        if (gDuel_bOpponentID >= 0) {
            File_RequestAsyncTransfer(
                0, 0,
                gDuel_bOpponentID * (DUELIST_DATA_SECTOR_COUNT - 1) +
                    gDuel_bOpponentID + DUELIST_DATA_FIRST_SECTOR,
                DUELIST_DATA_SECTOR_COUNT, 0, 0,
                (s32)gDuel_awOpponentDeckPool
            );
        }
        D_8009B1D5 = 0;
        gDuel_wSceneStateFlags = 1;
        func_8001778C();
        func_80017708();
        func_800175A0();
        D_8009B16C |= 0x1000;
    }
    D_8009B1C8 = (DuelSideState *)((u8 *)D_800E9FF0 + D_8009B1D5 * sizeof(DuelSideState));
    func_800178BC();
    D_800F2848.angle =
        D_8009B1D5 * TRIG_ANGLE_HALF_TURN + TRIG_ANGLE_QUARTER_TURN;
    func_8001352C();
    Duel_ClearHandSlots();
    pane = &D_800EA0E8[0];
    DuelEffect_ResetRequestPool();
    func_80029574(0);
    pane->src_x = 0;
    pane->src_y = 256;
    pane->field_2C = 0;
    pane->field_2E = 255;
    func_80029574(1);
    pane[1].src_x = 64;
    pane[1].src_y = 256;
    pane[1].field_2C = 0;
    pane[1].field_2E = 254;
    func_80035668(0);
    func_8001755C();
    File_WaitForTransfers();
    D_8009B22C =
        &D_800907D8[D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT];
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
    D_8009B214 = obj;
    obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(
        obj, 280, 32, 4, side, 0, 11, 748
    );
    DisplayObject_SelectOrderingTable1(obj);
    *(s16 *)&obj->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    if (D_8009B1D5 != 0) {
        *(u16 *)&obj->field_40.h.field_40 += 16;
    }
    D_8009B21C = obj;
    obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);
    DisplayObject_SelectOrderingTable1(obj);
    DisplayObject_SetDepthOffset((u8 *)obj, 1);
    obj->field_4C = (s32)Duel_DrawLifePointsAndDeckCounts;
    prev = (u8 *)D_8009B21C;
    obj->field_50.word = (s32)prev;
    D_800E9DB0[3] = func_800164FC;
    if (D_8009B369 != 1) {
        p = 0;
        D_8009B1DC = 0;
        D_8009B1D8 = 0;
        if (pid[-1] < 0) {
            if (gDuel_bOpponentID < 0) {
                D_8009B1D8 = D_801D1200;
                D_8009B1DC = D_801D1200 + 0x1000;
                Duel_ShuffleBothDecks(D_801D1200, D_801D1200 + 0x1000);
                return;
            }
            p = (u8 *)gDuel_awPlayerDeck;
            D_8009B1D8 = p;
            if (gDuel_bOpponentID < DUEL_MASTER_K_OPPONENT_ID) {
                q = 0;
                goto shuffle;
            }
        }
        q = p;
    shuffle:
        Duel_ShuffleBothDecks(p, q);
    }
}

void func_80017DB4(DuelCardDisplayObject *object)
{
    DuelCardRecord *card = &D_801A7AD8[object->card_index];

    if ((*(s32 *)&card->terrain_modifier & 0xA0000000) != 0xA0000000) {
        return;
    }
    if (D_8009B1C8->card_view_mode != 0) {
        object->field_67 = ((DuelCardDisplayData *)card->data)->field_04 + 1;
    }
    if (D_8009B1C8->card_view_mode < 0) {
        object->field_67 = 0xFF;
    }
}

void func_80017E3C(DuelCardDisplayObject *object)
{
    DuelCardRecord *card = &D_801A7AD8[object->card_index];
    u16 flags;

    if (!(card->flags & DUEL_CARD_FLAG_DISPLAY_MARKER)) {
        object->field_67 = 0;
    }
    flags = object->flags & ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
    object->flags = flags;
    if (card->flags &
        (DUEL_CARD_FLAG_DEFENSE_POSITION | DUEL_CARD_FLAG_FACE_DOWN)) {
        object->flags = flags | DISPLAY_OBJECT_FLAG_CLIP_TEST;
        object->field_21 = 0;
        if (card->flags & DUEL_CARD_FLAG_FACE_DOWN) {
            object->field_21 = 0x80;
        }
        object->field_22 = 0;
        if (card->flags & DUEL_CARD_FLAG_DEFENSE_POSITION) {
            object->field_22 = 0xC0;
        }
    }
    object->color = DUEL_DISPLAY_COLOR_NORMAL;
    if (card->flags & DUEL_CARD_FLAG_USED_THIS_TURN) {
        object->color = DUEL_DISPLAY_COLOR_DIMMED;
    }
}

u8 *func_80017F04(DuelCardRecord *arg0, s32 arg1, s32 arg2)
{
    DuelCardDisplayObject *p =
        DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);
    s32 *tbl;
    s32 k;

    k = arg0->card_id - 1;
    tbl = gDuel_adwCardStats;
    p->field_67 = 0;
    p->field_68 = (tbl[k] >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
    p->field_69 = 0;
    p->card_index =
        ((u32)arg0 - (u32)D_801A7AD8) / DUEL_CARD_RECORD_SIZE;
    p->field_6B = ((u8 *)arg0->data)[2];
    p->out_x = arg1;
    p->out_y = arg2;
    p->attribute |= DISPLAY_OBJECT_ATTRIBUTE_8BPP;
    p->field_10 = func_80016778;
    DisplayObject_SelectOrderingTable1((DisplayObject *)p);
    p->field_4C = Duel_DrawCardAtOutputPosition;
    func_80017E3C(p);
    func_80017DB4(p);
    return (u8 *)p;
}
