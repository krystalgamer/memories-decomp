#include "../types.h"
#include "card_constants.h"
#include "duel_card.h"

extern u8 D_8009B1D5;
extern s32 gDuel_adwCardStats[];
extern u8 D_800EAE88[];

extern s32 Duel_CheckEquip(s32 card_a, s32 card_b);
extern s32 func_80026C0C(s32 arg0);

s32 Duel_CollectFieldCardsBelowType(u8 **out, s32 arg1, s32 arg2)
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
                    *out++ = (u8 *)r;
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

s32 Duel_CollectFieldCardsByType(u8 **out, s32 arg1, s32 arg2) {
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
                    *out++ = (u8 *)r;
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
    u8 *a[DUEL_FIELD_ROW_SIZE + 1];
    u8 *b[DUEL_FIELD_ROW_SIZE + 1];
    u8 **q;
    u8 **r;
    u8 *e;
    u8 *f;
    s32 n;
    s32 u;
    s32 w;

    if (Duel_CollectFieldCardsByType(b, 0, 0x17) != 0) {
        Duel_CollectFieldCardsByType(a, DUEL_FIELD_ROW_SIZE, -1);
        e = b[0];
        if (e != (u8 *)0) {
            r = b;
            do {
                f = a[0];
                if (f != (u8 *)0) {
                    q = a;
                    do {
                        if (Duel_CheckEquip(*(s16 *)(e + 0xC),
                                            *(s16 *)(f + 0xC)) != 0) {
                            D_800EAE88[0] =
                                *(s8 *)(e + 0x18) % DUEL_FIELD_ROW_SIZE + 0xB;
                            D_800EAE88[1] = 0;
                            D_800EAE88[6] =
                                *(s8 *)(f + 0x18) % DUEL_FIELD_ROW_SIZE + 1;
                            D_800EAE88[7] = 0;
                            D_800EAE88[8] = 0;
                            return 0;
                        }
                        q++;
                        f = *q;
                    } while (f != (u8 *)0);
                }
                r++;
                e = *r;
            } while (e != (u8 *)0);
        }

        n = func_80026C0C(DUEL_FIELD_ROW_SIZE);
        if (n < 0) {
            return 1;
        }
        Duel_CollectFieldCardsBelowType(a, 0, 0x14);
        e = b[0];
        if (e != (u8 *)0) {
            r = b;
            do {
                f = a[0];
                if (f != (u8 *)0) {
                    q = a;
                    do {
                        if (Duel_CheckEquip(*(s16 *)(e + 0xC),
                                            *(s16 *)(f + 0xC)) != 0) {
                            u = *(s8 *)(f + 0x18);
                            D_800EAE88[0] =
                                u % DUEL_FIELD_ROW_SIZE + 0xB;
                            w = *(s8 *)(e + 0x18);
                            D_800EAE88[2] = 0;
                            D_800EAE88[7] = 0;
                            D_800EAE88[8] = 0;
                            D_800EAE88[6] =
                                n % DUEL_FIELD_ROW_SIZE + 1;
                            D_800EAE88[1] =
                                w % DUEL_FIELD_ROW_SIZE + 0xB;
                            return 0;
                        }
                        q++;
                        f = *q;
                    } while (f != (u8 *)0);
                }
                r++;
                e = *r;
            } while (e != (u8 *)0);
        }
    }
    return 1;
}
