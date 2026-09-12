#include "../types.h"
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
#include "display_object_api.h"
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
#include "func_80016D04.h"
#include "func_80016778.h"
#include "../unmatched.h"
#include "func_800179F4.h"
#include "func_8001755C.h"
#include "duel_effect_object_pool.h"
#include "func_80029574.h"
#include "text_render_state.h"
#include "func_800178BC.h"
#include "trig_constants.h"

/* The card display-object helper chain that follows the duel scene entry,
   func_800179F4, now a candidate in src/candidates/func_800179F4.c.
   func_80018004 immediately after this run calls the factory below, which
   applies both display-state helpers before returning the object. */

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
    DuelCardDisplayObject *p = func_800400AC(func_8004002C(), 6);
    s32 *tbl;
    s32 k;

    k = arg0->card_id - 1;
    tbl = gDuel_adwCardStats;
    p->field_67 = 0;
    p->field_68 = (tbl[k] >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
    p->field_69 = 0;
    p->card_index = ((u32)arg0 - (u32)D_801A7AD8) / DUEL_CARD_RECORD_SIZE;
    p->field_6B = ((u8 *)arg0->data)[2];
    p->out_x = arg1;
    p->out_y = arg2;
    p->attribute |= DISPLAY_OBJECT_ATTRIBUTE_8BPP;
    p->field_10 = func_80016778;
    func_80042918((DisplayObject *)p);
    p->field_4C = func_80016D04;
    func_80017E3C(p);
    func_80017DB4(p);
    return (u8 *)p;
}
