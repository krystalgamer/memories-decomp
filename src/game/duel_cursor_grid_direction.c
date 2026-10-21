#include "../types.h"
#include "../game/duel_side_state.h"
#include "../game/sound.h"
#include "../game/display_object_helpers.h"
#include "../game/duel_grid.h"
#include "../game/duel_side_view_angles.h"
#include "../game/duel_view_tween.h"
#include "../unmatched.h"
#include "../game/duel_cursor_grid_direction.h"
#define DUEL_FIELD_INFO_AMBIENT_SOURCE
#include "../game/duel_field_display_objects.h"

void DuelCursor_ProcessGridDirection(DuelSelectionRecord *o, s32 dir) {
    DisplayObject *d;
    s32 pos;
    s32 shift;
    s32 index;
    s32 q;

    d = o->cursor_object;
    if (o->status & 0x80) {
        if (gDuel_wDisplayTransitionFlags != 0) {
            return;
        }
        DuelField_RefreshCursorCardInfo();
        o->status &= 0x3F;
        SD_SEPlayFull(6);
        return;
    }
    if (dir < 0) {
        return;
    }
    if (dir & 1) {
        pos = o->row + 1;
        if (dir & 2) {
            pos = o->row - 1;
        }
        if (pos >= (s8)o->row_end) {
            return;
        }
        if (pos < (s8)o->row_min) {
            return;
        }
        shift = 0x10;
        if ((o->row ^ pos) != 3) {
            shift = 8;
        }
        Duel_StartViewTween(shift, 0x14E, 0x3FE, gDuel_awSideViewHeadings[gDuel_bActiveSide],
                      gDuel_awFieldGridRowViewTargetZ[gDuel_bActiveSide][o->field_18][pos]);
        o->row = pos;
        index = pos * DUEL_FIELD_ROW_SIZE + o->col;
        DisplayObject_ResetVelocity((DisplayObjectVelocity *)d);
        d->field_6C = 0;
        d->field_60 = shift;
        d->field_2C.h.field_2C = (s16)d->position.h.field_28;
        d->field_2C.h.field_2E = gDuel_aFieldGridGroundPositions[gDuel_bActiveSide][index].y;
        q = ((d->field_2C.h.field_2E - (s16)d->position.h.field_2A) << 8) / shift;
        gDuel_wDisplayTransitionFlags = 0x50;
        d->field_38.h.field_3A = q;
        o->status |= 0xC0;
    } else {
        pos = o->col + 1;
        if (dir & 2) {
            pos = o->col - 1;
        }
        if ((u32)pos >= DUEL_FIELD_ROW_SIZE) {
            return;
        }
        o->col = pos;
        index = o->row * DUEL_FIELD_ROW_SIZE + (s8)pos;
        DisplayObject_ResetVelocity((DisplayObjectVelocity *)d);
        shift = 8;
        d->field_60 = shift;
        d->field_6C = 0;
        d->field_2C.h.field_2C = gDuel_aFieldGridGroundPositions[gDuel_bActiveSide][index].x;
        gDuel_wDisplayTransitionFlags = 0x40;
        d->field_2C.h.field_2E = (s16)d->position.h.field_2A;
        d->field_34.h.field_36 =
            (((s16)d->field_2C.h.field_2C - (s16)d->position.h.field_28) << shift) / shift;
        o->status |= 0x80;
    }
}
