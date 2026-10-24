#include "../types.h"
#include "duel_card.h"
#include "card_grid_coordinates.h"

/* Neither helper has a caller the static tools can find: no C call, no jal
   and no stored pointer in the executable or any extracted DATA file. A
   runtime trace under #418 (interpreter CPU, 2,448 control hits) saw no call
   to either while visiting the main menu, the Library card list and a
   monster's 3-D view, and Build Deck, so those screens are ruled out; a duel
   and the Free Duel opponent grid were not visited. Later traces also saw
   no call on the idle title screen with its intro movie, in the Library 3-D
   view, or while moving the cursor over every zone of the duel board; see
   notes/trace-uncalled-functions.md. */

s32 CardGrid_GetColumnX(s32 value)
{
    if (value >= DUEL_FIELD_SIDE_ZONE_COUNT) {
        return (value % DUEL_FIELD_SIDE_ZONE_COUNT) * 14 + 174;
    }
    return (value % DUEL_FIELD_SIDE_ZONE_COUNT) * 14 + 14;
}

s32 CardGrid_GetRowY(s32 value)
{
    s32 quotient = value / DUEL_FIELD_SIDE_ZONE_COUNT;

    return quotient * 178 +
           (value - quotient * DUEL_FIELD_SIDE_ZONE_COUNT) * 16 + 14;
}
