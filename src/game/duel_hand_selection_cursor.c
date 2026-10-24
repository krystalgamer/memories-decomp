#include "../types.h"
#include "display_object_config.h"
#include "display_object_core.h"
#include "display_object.h"
#include "card_constants.h"
#include "duel_card_display_state.h"
#include "duel_selection_layout.h"
#include "sound.h"
#include "display_object_helpers.h"
#include "duel_hand_position_selection_cursor.h"
#include "duel_hand_selection_cursor.h"
#include "duel_display.h"
#include "duel_hand.h"
#include "duel_hand_selection_colors.h"

/* Regional values: the inner object's y (194 -> 210), and the count sprite's
 * field_5D (0xB8 -> 0xB0) and field_40, its CLUT x (0x250 -> 0x2D0). The
 * European build (src/game/european/) defines its own. */
#ifndef DUEL_HAND_STACK_INNER_Y
#define DUEL_HAND_STACK_INNER_Y 194
#endif
#ifndef DUEL_HAND_STACK_COUNT_FIELD_5D
#define DUEL_HAND_STACK_COUNT_FIELD_5D 0xB8
#endif
#ifndef DUEL_HAND_STACK_COUNT_CX
#define DUEL_HAND_STACK_COUNT_CX 0x250
#endif

#define DUEL_CARD_DISPLAY_OBJECT_VIEW(object) \
    ((DuelCardDisplayObject *)(object))
#define DISPLAY_OBJECT_VIEW(object) ((DisplayObject *)(object))

void DuelHand_PositionSelectionCursor(DuelSelectionRecord *object)
{
    DisplayObject *inner = object->cursor_object;

    inner->field_30.h.field_30 = (s8)object->field_0E * 60 + 14;
    inner->field_30.h.field_32 = DUEL_HAND_STACK_INNER_Y;
}

void DuelHand_CreateSelectionOrderMarker(DuelSelectionRecord *arg)
{
 register DuelSelectionRecord*object=arg;register DuelHandSlot*slot=&gDuelHand_aSlots[(s8)object->field_0E];register DuelCardDisplayObject*child;
 DUEL_CARD_DISPLAY_OBJECT_VIEW(slot->object)->out_y-=4;child=DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(),1);DisplayObject_ConfigureScreenSprite(DISPLAY_OBJECT_VIEW(child),DUEL_CARD_DISPLAY_OBJECT_VIEW(slot->object)->out_x,DUEL_CARD_DISPLAY_OBJECT_VIEW(slot->object)->out_y,0x10,0x10,object->field_15<<4,DUEL_HAND_STACK_COUNT_FIELD_5D,0xB,DUEL_HAND_STACK_COUNT_CX,0xFC);
 DisplayObject_SelectOrderingTable1(DISPLAY_OBJECT_VIEW(child));DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(child),(s8)((u8)slot->object->field_16+1));slot->order_marker=DISPLAY_OBJECT_VIEW(child);object->field_15++;slot->selection_order=object->field_15;SD_SEPlayFull(0x2F);
}

void DuelHand_RefreshSelectionColors(DuelSelectionRecord *side) {
    s32 i;
    for (i = 0; i < HAND_SIZE; i++)
        if (gDuelHand_aSlots[i].selection_order == 0)
            gDuelHand_aSlots[i].object->field_0C =
                DUEL_DISPLAY_COLOR_DIMMED;
    if (side->field_15 == 0)
        gDuelHand_aSlots[(s8)side->field_0E].object->field_0C =
            DUEL_DISPLAY_COLOR_NORMAL;
}
