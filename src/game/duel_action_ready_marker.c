#define D_8009B_DISPLAY_OBJECTS_VISIBLE
#define FUNC_80024088_WIDE_DIRECTION
#define DUEL_CURSOR_COORDS_SIGNED
#include "../types.h"
#include "display_object_core.h"
#include "display_object_layout.h"
#include "display_object_helpers.h"
#include "display_object_config.h"
#include "display_object.h"
#include "display_projection.h"
#include "../unmatched.h"
#include "duel_action_ready_marker.h"
#include "duel_grid.h"
#include "duel_cursor_status.h"
#include "duel_cursor_step_toward_target.h"

DisplayObject *Duel_GetOrCreateActionReadyMarker(DisplayObject *source)
{
    DisplayObject *object = gDuel_pActionReadyMarker;
    if (object == 0) {
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteResource(object, 4, 3, 8, 0xB, 0x1F0);
        object->field_6A = source->field_0A;
        DisplayObject_SetDepthOffset(object, 1);
        object->update = (DisplayObjectCallback)DisplayObject_ProjectLinkedGroundPosition;
        object->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    }
    return object;
}

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
