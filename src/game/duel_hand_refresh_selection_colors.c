#include "../types.h"
#include "card_constants.h"
#include "display_object.h"
#include "duel_display.h"
#include "duel_hand.h"
#include "duel_selection_layout.h"
#include "duel_hand_selection_colors.h"

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
