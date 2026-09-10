#include "../types.h"
#include "duel_side_state.h"
#include "sound.h"
#include "display_object_helpers.h"
#include "duel_grid.h"
#include "func_80022D94.h"
#include "../unmatched.h"
#include "func_80023D08.h"
#define FUNC_8002348C_AMBIENT_SOURCE
#include "duel_field_display_objects.h"

extern s16 D_800907AC[2][2][4];

void func_80023D08(GridCursor *o, s32 dir) {
    DuelFieldCursorObject *d;
    s32 pos;
    s32 shift;
    s32 index;
    register s32 q asm("$2");

    d = o->object;
    if (o->flags & 0x80) {
        if (D_8009B162 != 0) {
            return;
        }
        func_8002348C();
        o->flags &= 0x3F;
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
        if (pos >= o->max_row) {
            return;
        }
        if (pos < o->min_row) {
            return;
        }
        shift = 0x10;
        if ((o->row ^ pos) != 3) {
            shift = 8;
        }
        func_80022D94(shift, 0x14E, 0x3FE, D_8009AF20[D_8009B1D5],
                      D_800907AC[D_8009B1D5][o->page][pos]);
        o->row = pos;
        index = pos * DUEL_FIELD_ROW_SIZE + o->col;
        DisplayObject_ResetVelocity((DisplayObjectVelocity *)d);
        d->moving = 0;
        d->steps = shift;
        d->target_x = d->x;
        d->target_y = D_80090800[D_8009B1D5][index].y;
        q = ((d->target_y - d->y) << 8) / shift;
        D_8009B162 = 0x50;
        d->step_y = q;
        o->flags |= 0xC0;
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
        d->steps = shift;
        d->moving = 0;
        d->target_x = D_80090800[D_8009B1D5][index].x;
        D_8009B162 = 0x40;
        q = ((d->target_x - d->x) << shift) / shift;
        d->target_y = d->y;
        d->step_x = q;
        o->flags |= 0x80;
    }
}
