#include "../types.h"
#include "duel_side_state.h"
#include "duel_update_life_point_display.h"

/* Steps the drawn life-point counter one frame towards the real total. The
 * step grows with the distance left, which is what makes the readout race
 * for a large swing and crawl for a small one.
 */
void Duel_UpdateLifePointDisplay(DuelSideState *side)
{
    s32 difference =
        side->displayed_life_points - side->life_points.signed_value;
    /* This order and the builtin preserve the target a1/a2 register roles. */
    s32 step;
    s32 magnitude;

    if (difference == 0) {
        return;
    }
    magnitude = __builtin_abs(difference);
    step = 9;
    if (magnitude >= 300) {
        step = 19;
    }
    if (magnitude >= 1000) {
        step = 47;
    }
    if (magnitude >= 3000) {
        step = 97;
    }
    if (difference > 0) {
        difference -= step;
        if (difference < 0) {
            difference = 0;
        }
    } else {
        difference += step;
        if (difference > 0) {
            difference = 0;
        }
    }
    side->displayed_life_points =
        side->life_points.unsigned_value + difference;
}
