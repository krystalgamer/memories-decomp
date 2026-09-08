#include "../types.h"
#include "card_constants.h"
#include "duel_deck_card.h"
#include "file_transfer.h"
#include "duel_selection_layout.h"

typedef struct {
    u8 pad0000[8];
    u8 *hand;
} DuelSelectionSide;

extern u16 D_8009B23A;
extern u16 D_8009B162;
extern u8 D_8009B174;
extern u8 D_8009B1D5;
extern u8 D_8009B1EC;
extern u8 *D_8009B1B4;
extern u32 D_8009B0F4[];
extern u32 D_8009B134 __attribute__((section(".data")));
extern u16 D_8009B36A __attribute__((section(".data")));
extern u16 D_800F2848[];
extern u8 D_800E9F10[];
extern u8 D_800E9FF0[];
extern u8 D_800EA030[];
extern s32 gDuel_adwCardStats[];

extern void Duel_RequestCombinedDeckData(void);
extern void Duel_PopulateCombinedDeckData(void);
extern void Duel_ClearHandSlots(void);
extern void SD_BGMPlay(u32);
extern void func_8001352C(void);
extern void func_800157DC(void);

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

    w = D_800F2848;
    if ((D_8009B23A & 0x8000) == 0) {
        D_8009B23A |= 0x8000;
        Duel_RequestCombinedDeckData();
        D_800F2848[0] = 0x4B0;
        w[2] = 0x358;
        w[1] = 0x16C0;
        func_8001352C();
        D_8009B174 = 2;
        func_800157DC();
        return;
    }

    switch (D_8009B174 & 0x1F) {
    case 2:
        D_800F2848[0] -= 2;
        w[2] -= 2;
        w[1] -= 0x10;
        if ((s16)w[1] <= 0x400) {
            D_800F2848[0] = 0x258;
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

        if (((D_8009B0F4[0] & mask) |
             D_8009B134) != 0) {
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
        r1 = D_800E9FF0;
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
        r2 = D_800E9FF0;
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
