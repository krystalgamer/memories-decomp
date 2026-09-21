#define D_800EA030 gJapanese_DuelHandDisplayRecords
#include "../../types.h"
#include "../card_constants.h"
#include "../display_object.h"
#include "../duel_display.h"
#include "../duel_hand.h"
#include "../duel_selection_layout.h"
#include "../func_8001B8B8.h"

void func_8001B8B8(DuelSelectionRecord *side)
{
    s32 i;

    for (i = 0; i < HAND_SIZE; i++) {
        if (gJapanese_DuelHandDisplayRecords[i].active_09 == 0) {
            ((DisplayObject *)gJapanese_DuelHandDisplayRecords[i].object)
                ->field_0C = DUEL_DISPLAY_COLOR_DIMMED;
        }
    }
    if (side->field_15 == 0) {
        ((DisplayObject *)gJapanese_DuelHandDisplayRecords[(s8)side->field_0E]
             .object)
            ->field_0C = DUEL_DISPLAY_COLOR_NORMAL;
    }
}
