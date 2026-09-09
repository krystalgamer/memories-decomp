#include "../../types.h"
#include "trade_helpers.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "entrypoints.h"
#include "../../game/display_object_api.h"
#include "../../ygo_types.h"
#include "../../game/card_constants.h"

/* The Trade screen's offer display and its inventory bookkeeping: the offer
   and highlight draw (0x80183B2C), the three-digit number it prints
   (0x80183E8C), the display-handle release (0x80183FE4), the offer inventory
   delta (0x80184030) and the per-card count it applies (0x801840F8).

   The five are contiguous from 0x80183B2C to 0x8018416C and are the whole
   gcc_2_8_1_g0_split run there: card_stat_comparators.c below and
   card_name_count_comparators.c above both compile at
   gcc_2_8_1_g0_no_sched2_split, and a profile change cannot occur inside one
   unit, so both bounds are hard.

   They also call each other and share their data. The offer draw calls
   MainMenu_DrawThreeDigitNumber for every offered card, the offer delta calls
   MainMenu_AdjustTradeCardCount for every entry it walks, and the offer list
   D_80185C9C is read by the draw and written through by the delta. */

extern u8 *D_801845EC[];
extern u16 D_80185C9C[][11];
extern u8 D_80185CC8;

void MainMenu_DrawTradeOffersAndHighlights(void)
{
    u8 *volatile obj[2];
    s32 i;

    obj[0] = D_801845EC[0];
    obj[1] = D_801845EC[2];

    if (D_80185CC8 != 0) {
        MainMenu_DrawTradeColumnOverlay(0);
        obj[0][0xC] = obj[0][0xD] = obj[0][0xE] = 0x40;
        *(s16 *)(obj[0] + 0x60) = 2;
    } else {
        obj[0][0xC] += obj[0][0x60];
        obj[0][0xD] += obj[0][0x60];
        obj[0][0xE] += obj[0][0x60];
        if (obj[0][0xC] < 0x41 || (s8)obj[0][0xC] < 0) {
            *(s16 *)(obj[0] + 0x60) *= -1;
        }
    }

    if (D_80185CC9 != 0) {
        MainMenu_DrawTradeColumnOverlay(1);
        obj[1][0xC] = obj[1][0xD] = obj[1][0xE] = 0x40;
        *(s16 *)(obj[1] + 0x60) = 2;
    } else {
        obj[1][0xC] += obj[1][0x60];
        obj[1][0xD] += obj[1][0x60];
        obj[1][0xE] += obj[1][0x60];
        if (obj[1][0xC] < 0x41 || (s8)obj[1][0xC] < 0) {
            *(s16 *)(obj[1] + 0x60) *= -1;
        }
    }

    i = 0;
    if (D_80185C9C[0][0] != 0) {
        do {
            MainMenu_DrawThreeDigitNumber((i % 5) * 28 + 0x10, (i / 5) * 17 + 0xBC,
                          D_80185C9C[0][i + 1]);
            MainMenu_DrawCardTypeIcon((i % 5) * 28 + 0x10, (i / 5) * 17 + 0xBC,
                          D_80185C9C[0][i + 1]);
            i++;
        } while (i < D_80185C9C[0][0]);
    }

    i = 0;
    if (D_80185C9C[1][0] != 0) {
        do {
            MainMenu_DrawThreeDigitNumber((i % 5) * 28 + 0xB0, (i / 5) * 17 + 0xBC,
                          D_80185C9C[1][i + 1]);
            MainMenu_DrawCardTypeIcon((i % 5) * 28 + 0xB0, (i / 5) * 17 + 0xBC,
                          D_80185C9C[1][i + 1]);
            i++;
        } while (i < D_80185C9C[1][0]);
    }
}

extern GsOT *D_800E9D90[];

void MainMenu_DrawThreeDigitNumber(s32 x, s32 y, s32 value)
{
    POLY_FT4 sprite;
    s32 i;
    s32 quotient;
    s32 digit;
    s32 base;
    s32 left;
    s32 right;
    s32 u;
    s32 top;
    s32 bottom;

    i = 0;
    top = y + 4;
    bottom = y + 12;
    setPolyFT4(&sprite);
    sprite.tpage = 11;
    sprite.clut = 0x3EA9;
    sprite.r0 = 255;
    sprite.g0 = 255;
    sprite.b0 = 255;
    for (; i < 3; i++) {
        base = x + 24 - i * 8;
        left = base - 12;
        sprite.x0 = left;
        right = base - 4;
        sprite.x1 = right;
        sprite.x3 = right;
        quotient = value / 10;
        sprite.y0 = top;
        sprite.y1 = top;
        sprite.x2 = left;
        sprite.y2 = bottom;
        sprite.y3 = bottom;
        sprite.v0 = 112;
        sprite.v1 = 112;
        sprite.v2 = 120;
        sprite.v3 = 120;
        digit = value - quotient * 10;
        u = digit * 8;
        sprite.u0 = u - 128;
        sprite.u1 = u - 120;
        sprite.u2 = u - 128;
        sprite.u3 = u - 120;
        GsSortPoly(&sprite, D_800E9D90[1], 32);
        value = quotient;
    }
}

extern void *D_801845DC;
extern void *D_801845E0;
extern s32 D_800E9DB4;

void MainMenu_ReleaseTradeDisplayHandles(void)
{
    func_8004036C(D_801845DC);
    D_801845DC = 0;
    func_8004036C(D_801845E0);
    D_801845E0 = 0;
    D_800E9DB4 = 0;
}

void MainMenu_ApplyTradeOfferInventoryDelta(s32 slot, s32 amount)
{
    s32 i;

    i = 0;
    while (i < D_80185C9C[slot][0]) {
        MainMenu_AdjustTradeCardCount(slot, D_80185C9C[slot][i + 1], (u32)amount);
        i++;
    }
}

extern CardCountEntry D_801845FC[];

void MainMenu_AdjustTradeCardCount(s32 slot, s32 id, u32 amount)
{
    CardCountEntry *p;
    CardCountEntry *entry;
    u32 total;
    s32 offset;
    s32 i;

    i = 0;
    offset = slot * 2888;
    p = D_801845FC;
    while (i < CARD_COUNT) {
        entry = (CardCountEntry *)(offset + (s32)p);
        if (entry->id == id) {
            total = entry->count + amount;
            if (total < 0xFB) {
                entry->count = total;
            }
            return;
        }
        p++;
        i++;
    }
}
