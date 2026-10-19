#include "../types.h"
#include "../game/duel_side_state.h"
#include "../game/sound.h"
#include "../game/display_object_helpers.h"
#include "../game/duel_grid.h"
#include "../game/duel_side_view_angles.h"
#include "../game/func_80022D94.h"
#include "../unmatched.h"
#include "../game/func_80023D08.h"
#define FUNC_8002348C_AMBIENT_SOURCE
#include "../game/duel_field_display_objects.h"

void DuelCursor_ProcessGridDirection(DuelSelectionRecord *o, s32 dir) {
    DisplayObject *d;
    s32 pos;
    s32 shift;
    s32 index;
    s32 q;

    d = o->cursor_object;
    if (o->status & 0x80) {
        if (D_8009B162 != 0) {
            return;
        }
        func_8002348C();
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
        if (pos >= (s8)o->field_12) {
            return;
        }
        if (pos < (s8)o->field_11) {
            return;
        }
        shift = 0x10;
        if ((o->row ^ pos) != 3) {
            shift = 8;
        }
        Duel_StartViewTween(shift, 0x14E, 0x3FE, D_8009AF20[D_8009B1D5],
                      D_800907AC[D_8009B1D5][o->field_18][pos]);
        o->row = pos;
        index = pos * DUEL_FIELD_ROW_SIZE + o->col;
        DisplayObject_ResetVelocity((DisplayObjectVelocity *)d);
        d->field_6C = 0;
        d->field_60 = shift;
        d->field_2C.h.field_2C = (s16)d->position.h.field_28;
        d->field_2C.h.field_2E = gDuel_aFieldGridGroundPositions[D_8009B1D5][index].y;
        q = ((d->field_2C.h.field_2E - (s16)d->position.h.field_2A) << 8) / shift;
        D_8009B162 = 0x50;
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
        d->field_2C.h.field_2C = gDuel_aFieldGridGroundPositions[D_8009B1D5][index].x;
        D_8009B162 = 0x40;
        d->field_2C.h.field_2E = (s16)d->position.h.field_2A;
        d->field_34.h.field_36 =
            (((s16)d->field_2C.h.field_2C - (s16)d->position.h.field_28) << shift) / shift;
        o->status |= 0x80;
    }
}
