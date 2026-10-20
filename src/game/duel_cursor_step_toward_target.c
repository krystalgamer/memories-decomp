#include "../types.h"
#include "duel_grid.h"
#define FUNC_80024088_WIDE_DIRECTION
#define DUEL_CURSOR_COORDS_SIGNED
#include "duel_cursor_status.h"
#include "../unmatched.h"
#include "duel_cursor_step_toward_target.h"

s32 DuelCursor_StepTowardTarget(DuelSelectionRecord *cursor)
{
    if (gDuel_wDisplayTransitionFlags != 0) { gDuelCursor_Direction = -1; return 1; }
    if (DuelCursor_UpdateDirection(cursor, gDuelCursor_Direction) != 0) { return 1; }
    gDuelCursor_Direction = -1;
    if (gDuelCursor_TargetRow != cursor->row) {
        gDuelCursor_Direction = 1;
        if (gDuelCursor_TargetRow < cursor->row) { gDuelCursor_Direction = 3; }
        return 1;
    } else if (gDuelCursor_TargetColumn != cursor->col) {
        gDuelCursor_Direction = 0;
        if (gDuelCursor_TargetColumn < cursor->col) { gDuelCursor_Direction = 2; }
        return 1;
    } else {
        return 0;
    }
    return 1;
}
