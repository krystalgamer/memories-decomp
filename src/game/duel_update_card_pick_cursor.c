#define GINPUT_PAD1_HELD_IS_AGGREGATE
#define D_8009B254_IN_DATA
#include "../types.h"
#include "duel_side_state.h"
#include "duel_effect.h"
#include "duel_grid.h"
#include "duel_card_pick_cursor.h"
#include "duel_cursor_status.h"
#include "duel_card.h"
#include "func_80017034.h"
#include "input.h"
#include "../unmatched.h"

/* Per-frame step for the "pick a card off the field" cursor.
 *
 * D_8009B1D4 (0x2CC($gp)) is the mode byte for this cursor:
 *   0x80  the cursor has been initialised for this activation
 *   0x40  a pick has just been committed; hold until D_8009B162 expires
 *   0x20  the whole cursor mode should be torn down once 0x40 clears
 * D_8009B162 (0x25A($gp)) is the countdown that gates the 0x40 hold.
 *
 * First entry (0x80 clear) arms the cursor: mode becomes 0x80|0x40, the
 * object's own state fields are reset, and the hold counter is set to 3.
 *
 * While 0x40 is set the cursor is frozen: nothing happens until the counter
 * reaches 0, then 0x40 is cleared and -- if 0x20 was also set -- the whole
 * mode byte is zeroed, ending the cursor.
 *
 * Otherwise the cursor is live. func_80024060 refreshes the object's status
 * byte and returns nonzero while it is still busy; when it is idle, the cell
 * under the cursor is looked up in the field table (row * DUEL_FIELD_ROW_SIZE
 * + column, plus DUEL_FIELD_SIDE_GRID_SLOT_COUNT per player side) and the
 * record it names is offered to card_pick_on_up. A nonzero result is a
 * successful pick: it is published in gDuel_wViewerCardID along with the
 * event code 0x14 and state 2. A zero result only re-arms the hold (counter
 * 0xC, mode |= 0x60) when neither L2 nor R2 is held.
 */

/* gp-relative in the target (0x2CC/0x2CD/0x25A($gp)), so plain scalars. */

/* Absolute in the target, so array-typed to keep them out of small data. */

/* Neither call site narrows the result: each jal is followed by its delay
   slot and then a branch on $v0, with no andi or sll between, so the widened
   value is the callee's own. func_80024060's listing ends `lbu $v0,
   0x19($s0)` and really returns u8; func_80017034's non-zero exit is `lh $v0,
   0xC($a2)` and really returns s16. Both are declared s32, in
   duel_cursor_status.h and func_80017034.h, which this file includes. */
void Duel_UpdateCardPickCursor(DuelCardPickCursor *o) {
    u8 f;
    s32 picked;

    f = D_8009B1D4;
    if (!(f & 0x80)) {
        D_8009B1D4 = f | 0xC0;
        o->field_12 = 4;
        o->field_0C = 0x74;
        o->field_18 = 0;
        o->field_11 = 0;
        o->status = 0;
        D_8009B162 = 3;
    }
    f = D_8009B1D4;
    if (f & 0x40) {
        if (D_8009B162 == 0) {
            D_8009B1D4 = f & 0xBF;
            if (f & 0x20) {
                D_8009B1D4 = 0;
            }
        }
    } else if (func_80024060((DuelCursorStatus *)o) == 0) {
        picked = func_80017034(
            &D_801A7AD8[D_800907D8[
                o->row * DUEL_FIELD_ROW_SIZE + o->col +
                D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT
            ]]);
        if (picked != 0) {
            /* Written as absolute literals, not through externs: these two
               are single incidental stores and the target addresses them
               through $at, which the symbolic form does not do. The third
               store, to D_8009B254, reaches the same $at form through the
               .data arm duel_effect.h declares for it. */
            *(u16 *) 0x8009B246 = picked;
            *(u8 *) 0x8009B24B = 0x14;
            D_8009B254 = 2;
        } else if (!(gInput_wPad1Held[0] & PAD_BUTTON_TRIGGER_MASK)) {
            D_8009B162 = 0xC;
            D_8009B1D4 = D_8009B1D4 | 0x60;
        }
    }
}
