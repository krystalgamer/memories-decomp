#include "../types.h"
#include "display_object_layout.h"
#include "duel_card.h"
#include "duel_display.h"
#include "duel_card_display_state.h"
#include "duel_apply_card_object_flags.h"

/* Same D_801A7AD8[] stat table (0x1C-byte stride) as
   obj_apply_table801a7ad8_flags.c / table801a7ad8_row_search.c, but with the
   field_21/field_22 bit mapping SWAPPED relative to that sibling: here the
   face-down flag marks field_22, while the defense-position flag marks
   field_21.

   Clears the display flags' clip-test bit and field_22, then re-derives
   field_22 and field_21 from the selected D_801A7AD8 record's face-down and
   defense-position flags; always sets color to DUEL_DISPLAY_COLOR_NORMAL, or
   DUEL_DISPLAY_COLOR_DIMMED if DUEL_CARD_FLAG_USED_THIS_TURN is set; runs
   func_80017DB4, then clears field_67 unless
   DUEL_CARD_FLAG_DISPLAY_MARKER is set. */
void Duel_ApplyCardObjectFlags(DuelCardDisplayObject *object) {
    u16 flags = object->flags;
    s32 type = object->card_index;
    DuelCardRecord *rec;

    object->field_22 = 0;
    object->flags = flags & ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
    rec = &D_801A7AD8[type];

    if (rec->flags & DUEL_CARD_FLAG_FACE_DOWN) {
        object->field_22 = DUEL_CARD_DISPLAY_FACE_DOWN_VALUE;
    }
    object->field_21 = 0;
    if (rec->flags & DUEL_CARD_FLAG_DEFENSE_POSITION) {
        object->field_21 = DUEL_CARD_DISPLAY_DEFENSE_VALUE;
    }
    object->color = DUEL_DISPLAY_COLOR_NORMAL;
    if (rec->flags & DUEL_CARD_FLAG_USED_THIS_TURN) {
        object->color = DUEL_DISPLAY_COLOR_DIMMED;
    }
    func_80017DB4(object);
    if (!(rec->flags & DUEL_CARD_FLAG_DISPLAY_MARKER)) {
        object->field_67 = 0;
    }
}
