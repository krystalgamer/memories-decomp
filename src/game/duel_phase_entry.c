#define FUNC_80018004_AMBIENT_POSITION_ARGS
#define D_8009B36A_IN_DATA
#include "../types.h"
#include "duel_card.h"
#include "duel_side_state.h"
#include "duel_phase_entry.h"
#include "view_state.h"
#include "card_constants.h"
#include "duel_hand.h"
#include "duel_deck_card.h"
#include "fade.h"
#include "file_transfer.h"
#include "duel_selection_layout.h"

typedef struct {
    u8 pad0000[8];
    DuelHandSlot *hand;
} DuelSelectionSide;
extern u8 D_8009B1EC;
extern u8 *D_8009B1B4;

extern void Duel_RequestCombinedDeckData(void);
extern void Duel_PopulateCombinedDeckData(void);
#include "duel_card_layout.h"
#include "duel_grid.h"
#include "sound.h"
#include "../unmatched.h"
#include "func_80018004.h"
#include "duel_apply_card_object_flags.h"

extern u8 *D_8009B21C;

void func_80018608(void)
{
    u16 *w;
    u8 *r1;
    u8 *r2;
    DuelDeckCardRecord *rec;
    s32 i;
    s32 atk;
    s32 def;
    s32 stat;
    s32 stat2;

    w = (u16 *)&D_800F2848;
    if ((D_8009B23A & 0x8000) == 0) {
        D_8009B23A |= 0x8000;
        Duel_RequestCombinedDeckData();
        D_800F2848.field_00 = 0x4B0;
        w[2] = 0x358;
        w[1] = 0x16C0;
        func_8001352C();
        D_8009B174 = 2;
        Fade_StartIn();
        return;
    }

    switch (D_8009B174 & 0x1F) {
    case 2:
        D_800F2848.field_00 -= 2;
        w[2] -= 2;
        w[1] -= 0x10;
        if ((s16)w[1] <= 0x400) {
            D_800F2848.field_00 = 0x258;
            w[2] = 0x100;
            w[1] = 0x400;
            D_8009B174 = 3;
        }
        func_8001352C();
        break;
    case 3:
        if ((D_8009B174 & 0x80) == 0) {
            D_8009B174 |= 0x80;
            D_8009B162 = 2;
            D_8009B1B4 = D_800E9F10 + D_8009B1D5 * DUEL_SELECTION_SIDE_SIZE;
            *(u16 *)(D_8009B1B4 + 0xC) = 0xAE;
        }
        if (D_8009B162 == 0) {
            D_8009B174 = 4;
        }
        break;
    case 4: {
        register u32 mask asm("$2") = FILE_TRANSFER_REQUEST_BLOCKED_MASK;

        if (((D_8009B0F4_abs & mask) |
             D_8009B134_abs) != 0) {
            break;
        }
        SD_BGMPlay(D_8009B36A);
        Duel_PopulateCombinedDeckData();
        atk = 0;
        def = 0;
        rec = gDuel_aDeckCardRecords;
        for (i = 0; i < DECK_SIZE; i++, rec++) {
            stat = gDuel_adwCardStats[rec->id - 1];
            atk += (stat & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
            def += ((stat >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) *
                   CARD_STAT_SCALE;
        }
        r1 = (u8 *)D_800E9FF0;
        *(s16 *)(r1 + 0xE) = atk / DECK_SIZE;
        *(s16 *)(r1 + 0x10) = def / DECK_SIZE;
        atk = 0;
        def = 0;
        rec = &gDuel_aDeckCardRecords[DECK_SIZE];
        for (i = 0; i < DECK_SIZE; i++, rec++) {
            stat2 = gDuel_adwCardStats[rec->id - 1];
            atk += (stat2 & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
            def += ((stat2 >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) *
                   CARD_STAT_SCALE;
        }
        r2 = (u8 *)D_800E9FF0;
        *(s16 *)(r2 + 0x2E) = atk / DECK_SIZE;
        *(s16 *)(r2 + 0x30) = def / DECK_SIZE;
        D_8009B174 = 5;
        break;
    }
    case 5:
        Duel_ClearHandSlots();
        D_8009B1EC = 5;
        D_8009B23A = 3;
        ((DuelSelectionSide *)(D_800E9F10 +
            D_8009B1D5 * DUEL_SELECTION_SIDE_SIZE))->hand = D_800EA030;
        break;
    }
}

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
    DuelHandSlot *base;
    DuelHandSlot *slot;
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
        D_8009B1C8 = (DuelSideState *)((u8 *)D_800E9FF0 + side * sizeof(DuelSideState));
        D_8009B1B4 = D_800E9F10 + side * DUEL_SELECTION_SIDE_SIZE;
        base = D_800EA030;
        *(DuelHandSlot **)(D_8009B1B4 + 8) = base;
        if (*(s8 *)((u8 *)D_8009B1C8 + 0x19) != 0) {
            c = ((u8 *)D_8009B1C8)[0x19] - 1;
            ((u8 *)D_8009B1C8)[0x19] = c;
            if (c <= 0) {
                ((u8 *)D_8009B1C8)[0x19] = 0;
                *(u16 *)(D_8009B1F0[D_8009B1D5] + 0x1A) = 0xFFFD - D_8009B1D5;
                D_8009B1F0[D_8009B1D5] = 0;
            }
        }
        ((u8 *)D_8009B1C8)[1]++;
        rec = (u8 *)D_801A7AD8;
        for (i = 0; i < DUEL_CARD_RECORD_COUNT; i++, rec += DUEL_CARD_RECORD_SIZE) {
            flags = *(u16 *)(rec + 0x16);
            if (flags & DUEL_CARD_FLAG_OCCUPIED) {
                *(u16 *)(rec + 0x16) = flags & ~DUEL_CARD_FLAG_USED_THIS_TURN;
                Duel_ApplyCardObjectFlags(*(DuelCardDisplayObject **)rec);
            } else {
                *(u16 *)(rec + 0x16) = 0;
            }
        }
        for (i = 0; i < HAND_SIZE; i++) {
            hand[i] = ((u8 *)D_8009B1C8 + i)[0x1A];
            *(s8 *)((u8 *)D_8009B1C8 + i + 0x1A) = -1;
        }
        n = 0;
        i = n;
        slot = base;
        y = 0xE;
        idx = D_8009B1D5 * DUEL_CARD_SIDE_RECORD_COUNT;
        rec = (u8 *)D_801A7AD8 + idx * DUEL_CARD_RECORD_SIZE;
        placed = rec;
        for (; i < HAND_SIZE; i++, rec += DUEL_CARD_RECORD_SIZE) {
            p = &hand[i];
            *(u16 *)(rec + 0x16) = 0;
            *(u8 **)rec = 0;
            if (*(s8 *)p >= 0) {
                ((u8 *)D_8009B1C8 + n)[0x1A] = *p;
                Duel_SetupCardRecord(idx, *(s8 *)p);
                idx++;
                n++;
                slot->object = (u8 *)func_80018004(placed, y, 0x292);
                slot++;
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
