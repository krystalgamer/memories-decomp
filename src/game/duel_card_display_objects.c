#define FUNC_80017F04_AMBIENT_POSITION_ARGS
#include "../types.h"
#include "duel_card_object_helpers.h"
#include "display_object_core.h"
#include "card_constants.h"
#include "display_object_layout.h"
#include "display_object_helpers.h"
#include "duel_side_state.h"
#include "duel_card.h"
#include "duel_deck_card.h"
#include "duel_card_layout.h"
#include "duel_card_display_state.h"
#include "duel_card_display_objects.h"
#include "duel_display.h"

DuelCardDisplayObject *Duel_CreateHandCardDisplayObject(DuelCardRecord *card)
{
    DuelCardDisplayObject *result =
        Duel_CreateCardDisplayObject((u8 *)card);

    if (gDuel_pActiveSideState->card_view_mode != 0) {
        card->flags |= DUEL_CARD_FLAG_DISPLAY_MARKER;
        result->field_67 =
            ((DuelDeckCardRecord *)card->data)->flags_04 + 1;
        if (gDuel_pActiveSideState->card_view_mode < 0) {
            result->field_67 = 0xFF;
        }
    }
    return result;
}

/* Same gDuel_aCardRecords[] stat table (0x1C-byte stride) as Duel_UpdateCardDisplayState, but with
   the field_21/field_22 bit mapping SWAPPED relative to it: here the face-down
   flag marks field_22, while the defense-position flag marks field_21.

   Clears the display flags' clip-test bit and field_22, then re-derives
   field_22 and field_21 from the selected gDuel_aCardRecords record's face-down and
   defense-position flags; always sets color to DUEL_DISPLAY_COLOR_NORMAL, or
   DUEL_DISPLAY_COLOR_DIMMED if DUEL_CARD_FLAG_USED_THIS_TURN is set; runs
   Duel_UpdateCardDisplayMarker, then clears field_67 unless
   DUEL_CARD_FLAG_DISPLAY_MARKER is set. */
void Duel_ApplyCardObjectFlags(DuelCardDisplayObject *object) {
    u16 flags = object->flags;
    s32 type = object->card_index;
    DuelCardRecord *rec;

    object->field_22 = 0;
    object->flags = flags & ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
    rec = &gDuel_aCardRecords[type];

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
    Duel_UpdateCardDisplayMarker(object);
    if (!(rec->flags & DUEL_CARD_FLAG_DISPLAY_MARKER)) {
        object->field_67 = 0;
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
    if (object->flag != 0) value |= 0x80;
    return value;
}
