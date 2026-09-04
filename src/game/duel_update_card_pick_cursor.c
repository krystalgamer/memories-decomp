#include "../types.h"
#include "duel_card.h"

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
 * + column, plus 0x14 per player side) and the record it names is offered to
 * card_pick_on_up. A nonzero result is a successful pick: it is published in
 * gDuel_wViewerCardID along with the event code 0x14 and state 2. A zero
 * result only re-arms the hold (counter 0xC, mode |= 0x60) when no pad bit in
 * the low two bits of D_8009B3A4_arr is down.
 */

/* The cursor object the caller owns. 0xF/0x10 are the same column/row pair
   src/card_pick.c reads through D_8009B1B4. */
struct Cursor {
    char pad0[0xC];
    s16 fC;         /* 0xC  — reset to 0x74 when the cursor is armed */
    char pad1[0x1];
    s8 col;         /* 0xF */
    s8 row;         /* 0x10 */
    u8 f11;         /* 0x11 */
    u8 f12;         /* 0x12 */
    char pad2[0x5];
    u8 f18;         /* 0x18 */
    u8 f19;         /* 0x19 — the status byte func_80024060 returns */
};

/* gp-relative in the target (0x2CC/0x2CD/0x25A($gp)), so plain scalars. */
extern u8 D_8009B1D4;
extern u8 D_8009B1D5;
extern u16 D_8009B162;

/* Absolute in the target, so array-typed to keep them out of small data. */
extern u8 D_800907D8[];
extern u16 D_8009B3A4[];

/* Both callees are reached without a prototype in the original, so their
   results arrive in $v0 already widened -- there is no andi/sll narrowing at
   either call site. func_80024060 really returns u8 (see
   src/call_80023fbc_read_field25.c) and card_pick_on_up really returns s16
   (see src/card_pick.c). */
extern s32 func_80024060(struct Cursor *);
extern s32 func_80017034(DuelCardRecord *);

void Duel_UpdateCardPickCursor(struct Cursor *o) {
    u8 f;
    s32 picked;

    f = D_8009B1D4;
    if (!(f & 0x80)) {
        D_8009B1D4 = f | 0xC0;
        o->f12 = 4;
        o->fC = 0x74;
        o->f18 = 0;
        o->f11 = 0;
        o->f19 = 0;
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
    } else if (func_80024060(o) == 0) {
        picked = func_80017034(
            &D_801A7AD8[D_800907D8[
                o->row * DUEL_FIELD_ROW_SIZE + o->col + D_8009B1D5 * 0x14
            ]]);
        if (picked != 0) {
            /* Written as absolute literals, not through externs: these three
               are single incidental stores and the target addresses them
               through $at, which the symbolic form does not do. */
            *(u16 *) 0x8009B246 = picked;
            *(u8 *) 0x8009B24B = 0x14;
            *(u8 *) 0x8009B254 = 2;
        } else if (!(D_8009B3A4[0] & 3)) {
            D_8009B162 = 0xC;
            D_8009B1D4 = D_8009B1D4 | 0x60;
        }
    }
}
