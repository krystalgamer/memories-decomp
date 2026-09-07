#include "../types.h"
#include "card_constants.h"
#include "duel_card_layout.h"
#include "duel_grid.h"
#include "duel_selection_layout.h"

extern u16 D_8009B23A;
extern u8 *D_8009B21C;
extern u8 D_8009B1D5;
extern u8 *D_8009B1C8;
extern u8 *D_8009B1B4;
extern u8 *D_8009B1F0[DUEL_SIDE_COUNT];
extern u8 D_8009B1EC;
extern u16 D_8009B162;
extern u8 D_800E9F10[];
extern u8 D_800E9FF0[];
extern u8 D_800EA030[];
extern u8 D_801A7AD8[];

extern void Duel_ClearHandSlots(void);
extern void Duel_ApplyCardObjectFlags(u8 *);
extern u8 *Duel_SetupCardRecord(s32, s32);
extern u8 *func_80018004(u8 *, s32, s32);

/* Draw phase entry. On its first call it sets the phase flag, points the
   side state and selection records at the current side, ticks down the
   side's pending counter (releasing the pending object at D_8009B1F0 when
   it reaches zero), resets the used flag on every card record, then
   rebuilds the hand: the five hand slot bytes are copied out and cleared,
   each valid one gets its card record set up and a hand object spawned at
   a fixed x and a 0x3C-spaced y, and the number of empty slots is stored
   for the draw. Later calls only clear the phase flag once the message
   state has returned to zero. */
void func_8001898C(void) {
    u8 hand[HAND_SIZE];
    u8 *rec;
    u8 *fl;
    u8 *placed;
    u8 *base;
    u8 *slot;
    u8 *p;
    s32 side;
    s32 i;
    s32 n;
    s32 idx;
    s32 y;
    u16 flags;
    s8 c;

    if ((D_8009B23A & 0x8000) == 0) {
        D_8009B23A |= 0x8000;
        *(u16 *)(D_8009B21C + 0x40) = (D_8009B1D5 << 4) | 0x2E0;
        Duel_ClearHandSlots();
        side = D_8009B1D5;
        D_8009B1C8 = D_800E9FF0 + side * 0x20;
        D_8009B1B4 = D_800E9F10 + side * DUEL_SELECTION_SIDE_SIZE;
        base = D_800EA030;
        *(u8 **)(D_8009B1B4 + 8) = base;
        if (*(s8 *)(D_8009B1C8 + 0x19) != 0) {
            c = D_8009B1C8[0x19] - 1;
            D_8009B1C8[0x19] = c;
            if (c <= 0) {
                D_8009B1C8[0x19] = 0;
                *(u16 *)(D_8009B1F0[D_8009B1D5] + 0x1A) = 0xFFFD - D_8009B1D5;
                D_8009B1F0[D_8009B1D5] = 0;
            }
        }
        D_8009B1C8[1]++;
        rec = D_801A7AD8;
        for (i = 0; i < DUEL_CARD_RECORD_COUNT; i++, rec += DUEL_CARD_RECORD_SIZE) {
            flags = *(u16 *)(rec + 0x16);
            if (flags & DUEL_CARD_FLAG_OCCUPIED) {
                *(u16 *)(rec + 0x16) = flags & ~DUEL_CARD_FLAG_USED_THIS_TURN;
                Duel_ApplyCardObjectFlags(*(u8 **)rec);
            } else {
                *(u16 *)(rec + 0x16) = 0;
            }
        }
        for (i = 0; i < HAND_SIZE; i++) {
            hand[i] = (D_8009B1C8 + i)[0x1A];
            *(s8 *)(D_8009B1C8 + i + 0x1A) = -1;
        }
        n = 0;
        i = n;
        slot = base;
        y = 0xE;
        idx = D_8009B1D5 * DUEL_CARD_SIDE_RECORD_COUNT;
        rec = D_801A7AD8 + idx * DUEL_CARD_RECORD_SIZE;
        placed = rec;
        for (; i < HAND_SIZE; i++, rec += DUEL_CARD_RECORD_SIZE) {
            p = &hand[i];
            *(u16 *)(rec + 0x16) = 0;
            *(u8 **)rec = 0;
            if (*(s8 *)p >= 0) {
                (D_8009B1C8 + n)[0x1A] = *p;
                Duel_SetupCardRecord(idx, *(s8 *)p);
                idx++;
                n++;
                *(u8 **)slot = func_80018004(placed, y, 0x292);
                slot += 0xC;
                y += 0x3C;
                placed += DUEL_CARD_RECORD_SIZE;
            }
        }
        D_8009B1EC = HAND_SIZE - n;
        D_8009B162 = 2;
        D_8009B1B4 = D_800E9F10 + D_8009B1D5 * DUEL_SELECTION_SIDE_SIZE;
        *(u16 *)(D_8009B1B4 + 0xC) = 0xAE;
    } else if (D_8009B162 == 0) {
        D_8009B23A = 3;
    }
}
