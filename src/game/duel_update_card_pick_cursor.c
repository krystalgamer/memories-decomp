#define GINPUT_PAD1_HELD_IS_AGGREGATE
#define GDUEL_BEFFECTSTATE_IN_DATA
#include "../types.h"
#include "duel_side_state.h"
#include "duel_effect.h"
#include "duel_grid.h"
#include "duel_card_pick_cursor.h"
#include "duel_cursor_status.h"
#include "duel_card.h"
#include "duel_get_card_viewer_request_id.h"
#include "input.h"
#include "../unmatched.h"
#include "duel_field_card_render_data.h"

/* Regional value: the cursor's field_0C (0x74 -> 0x84). The European build
 * (src/game/european/) defines its own. */
#ifndef DUEL_PICK_CURSOR_FIELD_0C
#define DUEL_PICK_CURSOR_FIELD_0C 0x74
#endif

/* The absolute spellings below are load-bearing (see the two stores in
 * Duel_UpdateCardPickCursor); the addresses are named so a regional build can
 * supply its own. */
#ifndef DUEL_VIEWER_CARD_ID_ADDRESS
#define DUEL_VIEWER_CARD_ID_ADDRESS 0x8009B246
#endif

#ifndef DUEL_VIEWER_Y_OFFSET_ADDRESS
#define DUEL_VIEWER_Y_OFFSET_ADDRESS 0x8009B24B
#endif

/* Per-frame step for the "pick a card off the field" cursor.
 *
 * gDuelCursor_PickFlags (0x2CC($gp)) is the mode byte for this cursor:
 *   0x80  the cursor has been initialised for this activation
 *   0x40  a pick has just been committed; hold until the display request clears
 *   0x20  the whole cursor mode should be torn down once 0x40 clears
 * gDuel_wDisplayTransitionFlags (0x25A($gp)) is the request/status word
 * that gates the 0x40 hold, not a separate countdown.
 *
 * First entry (0x80 clear) arms the cursor: mode becomes 0x80|0x40, the
 * object's own state fields are reset, and display request 0x3 is started.
 *
 * While 0x40 is set the cursor is frozen: nothing happens until the request
 * clears, then 0x40 is cleared and -- if 0x20 was also set -- the whole
 * mode byte is zeroed, ending the cursor.
 *
 * Otherwise the cursor is live. DuelCursor_UpdateFromInput refreshes the
 * object's status byte and returns nonzero while it is still busy; when it is
 * idle, the cell under the cursor is looked up in the field table (row *
 * DUEL_FIELD_ROW_SIZE + column, plus DUEL_FIELD_SIDE_GRID_SLOT_COUNT per
 * player side) and the record it names is offered to
 * Duel_GetCardViewerRequestId. A nonzero result is a successful pick: it is
 * published in gDuel_wViewerCardID along with the
 * event code 0x14 and state 2. A zero result only re-arms the hold (display
 * request 0xC, mode |= 0x60) when neither L2 nor R2 is held.
 */

/* gp-relative in the target (0x2CC/0x2CD/0x25A($gp)), so plain scalars. */

/* Absolute in the target, so array-typed to keep them out of small data. */

/* Neither call site narrows the result: each jal is followed by its delay
   slot and then a branch on $v0, with no andi or sll between, so the widened
   value is the callee's own. DuelCursor_UpdateFromInput's listing ends `lbu $v0,
   0x19($s0)` and really returns u8; Duel_GetCardViewerRequestId's non-zero exit is `lh $v0,
   0xC($a2)` and really returns s16. Both are declared s32, in
   duel_cursor_status.h and duel_get_card_viewer_request_id.h, which this file includes. */
void Duel_UpdateCardPickCursor(DuelSelectionRecord *o) {
    u8 f;
    s32 picked;

    f = gDuelCursor_PickFlags;
    if (!(f & 0x80)) {
        gDuelCursor_PickFlags = f | 0xC0;
        o->row_end = 4;
        o->field_0C = DUEL_PICK_CURSOR_FIELD_0C;
        o->field_18 = 0;
        o->row_min = 0;
        o->status = 0;
        gDuel_wDisplayTransitionFlags = 3;
    }
    f = gDuelCursor_PickFlags;
    if (f & 0x40) {
        if (gDuel_wDisplayTransitionFlags == 0) {
            gDuelCursor_PickFlags = f & 0xBF;
            if (f & 0x20) {
                gDuelCursor_PickFlags = 0;
            }
        }
    } else if (DuelCursor_UpdateFromInput(o) == 0) {
        picked = Duel_GetCardViewerRequestId(
            &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices[
                o->row * DUEL_FIELD_ROW_SIZE + o->col +
                gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT
            ]]);
        if (picked != 0) {
            /* Written as absolute literals, not through externs: these two
               are single incidental stores and the target addresses them
               through $at, which the symbolic form does not do. The third
               store, to gDuel_bEffectState, reaches the same $at form through the
               .data arm duel_effect.h declares for it. */
            *(u16 *) DUEL_VIEWER_CARD_ID_ADDRESS = picked;
            *(u8 *) DUEL_VIEWER_Y_OFFSET_ADDRESS = 0x14;
            gDuel_bEffectState = DUEL_EFFECT_STATE_CARD_VIEWER;
        } else if (!(gInput_wPad1Held[0] & PAD_BUTTON_TRIGGER_MASK)) {
            gDuel_wDisplayTransitionFlags = 0xC;
            gDuelCursor_PickFlags = gDuelCursor_PickFlags | 0x60;
        }
    }
}
