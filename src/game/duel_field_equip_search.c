#include "../types.h"
#include "ai.h"
#include "duel_side_state.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_field_equip_search.h"
#include "duel_card_checks.h"

s32 func_80026C0C(s32 arg0)
{
    /* off and p share a0 on purpose: retail scales base by the card-record
     * size into a0 and then adds the D_801A7AD8 symbol in place. The two live
     * ranges touch for exactly zero instructions, because
     * "p = D_801A7AD8 + off" reads off and writes p in a single instruction.
     * Do not insert any statement between the off and p assignments below;
     * that would make the shared pin invalid and the resulting mismatch is
     * not obvious from the source. */
    register s32 base asm("v1");
    register s32 off asm("a0");
    register u8 *p asm("a0");
    s32 i;

    base = D_8009B1D5 * DUEL_CARD_SIDE_RECORD_COUNT + arg0;
    off = base * DUEL_CARD_RECORD_SIZE;
    p = (u8 *)D_801A7AD8 + off;
    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
        if ((*(u16 *)(p + 0x16) & DUEL_CARD_FLAG_OCCUPIED) == 0) {
            return base + i;
        }
        p += DUEL_CARD_RECORD_SIZE;
    }
    return -1;
}

s32 Duel_CollectFieldCardsBelowType(DuelCardRecord **out, s32 arg1,
                                    s32 arg2)
{
    /* Retail keeps the result count in t0 and the five-entry index in a3. */
    register s32 count asm("$8") = 0;
    register s32 i asm("$7") = 0;
    s32 d = D_8009B1D5;
    s32 *t = gDuel_adwCardStats;
    DuelCardRecord *r =
        &D_801A7AD8[d * DUEL_CARD_SIDE_RECORD_COUNT + arg1];
    s32 k;

    do {
        /* Match the block numbering used by the adjacent
         * Duel_CollectFieldCardsByType. */
        do {
            if (r->flags & DUEL_CARD_FLAG_OCCUPIED) {
                k = (s16)r->card_id;
                k--;
                if (((t[k] >> CARD_STAT_TYPE_SHIFT) &
                     CARD_STAT_TYPE_MASK) < arg2) {
                    *out++ = r;
                    count++;
                }
            }
        } while (0);
        i++;
        r++;
    } while (i < DUEL_FIELD_ROW_SIZE);

    *out = 0;
    return count;
}

s32 Duel_CollectFieldCardsByType(DuelCardRecord **out, s32 arg1,
                                 s32 arg2) {
    s32 count = 0;
    s32 i = 0;
    s32 d = D_8009B1D5;
    s32 *t = gDuel_adwCardStats;
    DuelCardRecord *r =
        &D_801A7AD8[d * DUEL_CARD_SIDE_RECORD_COUNT + arg1];
    s32 k;

    do {
        /* The `do { } while (0)` is load-bearing and NOT a macro leftover:
         * it is 2 differences without it and 6 with `{ }` or `if (1) { }`
         * instead, because gcc's loop pass sees the extra loop node and
         * numbers the blocks differently. Coupled with `count` being
         * initialised before `i` -- that alone is 6. Found by the permuter;
         * see docs/PARKED.txt's former entry. */
        do {
            if (r->flags & DUEL_CARD_FLAG_OCCUPIED) {
                if (arg2 < 0 ||
                    (k = (s16)r->card_id, k--,
                     ((t[k] >> CARD_STAT_TYPE_SHIFT) &
                      CARD_STAT_TYPE_MASK) == arg2)) {
                    *out++ = r;
                    count++;
                }
            }
        } while (0);
        i++;
        r++;
    } while (i < DUEL_FIELD_ROW_SIZE);

    *out = 0;

    return count;
}

s32 func_80026DC8(void) {
    DuelCardRecord *a[DUEL_FIELD_ROW_SIZE + 1];
    DuelCardRecord *b[DUEL_FIELD_ROW_SIZE + 1];
    DuelCardRecord **q;
    DuelCardRecord **r;
    DuelCardRecord *e;
    DuelCardRecord *f;
    s32 n;
    s32 u;
    s32 w;

    if (Duel_CollectFieldCardsByType(b, 0, CARD_TYPE_EQUIP) != 0) {
        Duel_CollectFieldCardsByType(a, DUEL_FIELD_ROW_SIZE, -1);
        e = b[0];
        if (e != 0) {
            r = b;
            do {
                f = a[0];
                if (f != 0) {
                    q = a;
                    do {
                        if (Duel_CheckEquip(e->card_id, f->card_id) != 0) {
                            D_800EAE88.result =
                                *(s8 *)&e->table_index % DUEL_FIELD_ROW_SIZE + 0xB;
                            D_800EAE88.field1 = 0;
                            D_800EAE88.value =
                                *(s8 *)&f->table_index % DUEL_FIELD_ROW_SIZE + 1;
                            D_800EAE88.zero = 0;
                            D_800EAE88.random = 0;
                            return 0;
                        }
                        q++;
                        f = *q;
                    } while (f != 0);
                }
                r++;
                e = *r;
            } while (e != 0);
        }

        n = func_80026C0C(DUEL_FIELD_ROW_SIZE);
        if (n < 0) {
            return 1;
        }
        Duel_CollectFieldCardsBelowType(a, 0, CARD_TYPE_MAGIC);
        e = b[0];
        if (e != 0) {
            r = b;
            do {
                f = a[0];
                if (f != 0) {
                    q = a;
                    do {
                        if (Duel_CheckEquip(e->card_id, f->card_id) != 0) {
                            u = *(s8 *)&f->table_index;
                            D_800EAE88.result =
                                u % DUEL_FIELD_ROW_SIZE + 0xB;
                            w = *(s8 *)&e->table_index;
                            D_800EAE88.combo_tail[0] = 0;
                            D_800EAE88.zero = 0;
                            D_800EAE88.random = 0;
                            D_800EAE88.value =
                                n % DUEL_FIELD_ROW_SIZE + 1;
                            D_800EAE88.field1 =
                                w % DUEL_FIELD_ROW_SIZE + 0xB;
                            return 0;
                        }
                        q++;
                        f = *q;
                    } while (f != 0);
                }
                r++;
                e = *r;
            } while (e != 0);
        }
    }
    return 1;
}
