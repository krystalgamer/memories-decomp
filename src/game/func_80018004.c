#define FUNC_80017F04_AMBIENT_POSITION_ARGS
#include "../types.h"
#include "duel_side_state.h"
#include "duel_card.h"
#include "duel_card_layout.h"
#include "duel_card_display_state.h"
#include "func_80018004.h"

DuelCardDisplayObject *func_80018004(DuelCardRecord *card)
{
    DuelCardDisplayObject *result =
        (DuelCardDisplayObject *)func_80017F04((u8 *)card);

    if (D_8009B1C8->card_view_mode != 0) {
        card->flags |= DUEL_CARD_FLAG_DISPLAY_MARKER;
        result->field_67 =
            ((DuelCardDisplayData *)card->data)->field_04 + 1;
        if (D_8009B1C8->card_view_mode < 0) {
            result->field_67 = 0xFF;
        }
    }
    return result;
}
