#include "../../types.h"
#include "../../ygo_types.h"
#include "../../game/card_constants.h"
#include "entrypoints.h"
#include "trade_helpers.h"
#include "../../game/display_object_config.h"
#include "../../game/data_transfer_request.h"
#include "../../game/duel_card_viewer.h"
#include "../../game/duel_effect.h"

typedef struct { u32 words[256]; } Block1024;

typedef struct {
    u32 words[4];
} Block16;

extern MainMenuWidget *D_801845E0;
extern MainMenuWidget *D_801845EC;
extern MainMenuWidget *D_801845F4;
extern CardCountEntry D_801845FC[];
extern CardCountEntry D_80185144[];
extern u16 D_80185C8C[2][2];
extern u16 D_80185C9C[2][11];
extern u8 D_80185CC8[2];
extern u8 D_80185CCA[2];
extern u8 D_80185CCB;
extern u8 D_80185CCC[2];
extern Block16 D_801D1200[];
extern u8 D_800EB224[];
extern volatile u16 D_8009B394[2];
extern volatile u16 D_8009B398[2];
extern volatile u16 D_8009B3A4[2];
extern u16 D_8009B246;

extern void func_8003CB7C(void);
extern void func_8003FE14(void);
extern void func_800611D0(s32);
extern void func_800610E0(s32);
extern void func_80048658(s32, s32, s32);
extern void func_80035B7C(void *);
s32 MainMenu_UpdateTradeScreen(void)
{
    s32 bounded;
    s32 low;
    s32 decision;
    /* These bindings also preserve the initialization table's allocation. */
    register s32 dirty0 __asm__("$19");
    register s32 dirty1 __asm__("$18");
    s32 cursor0;
    s32 cursor1;
    s32 flags;
    s32 previousFlags;
    s32 maximum;
    s32 result;
    s32 i;
    s32 j;
    s32 id;
    s32 value;
    s32 other;
    u8 *counts[2];
    u8 *clearFlags;
    u8 *from;
    u8 *to;
    Block16 *base;
    Block16 *source;
    Block16 *save_source;
    Block16 *destination;
    Block16 *save_destination;
    Block16 *end;
    /* Preserve the second restore copy's addresses across the first walk. */
    register Block16 *source2 __asm__("$6");
    Block16 *backup_source2;
    register Block16 *destination2 __asm__("$5");
    Block16 *backup_destination2;
    CardCountEntry *card0;
    CardCountEntry *card1;

    dirty0 = 0;
    dirty1 = dirty0;
    cursor0 = 1;
    cursor1 = cursor0;
    value = D_801845E0->frame - 4;
    previousFlags = value;
    flags = value;
    maximum = 715;

    if (D_80185CD1 != 0) {
        result = func_8003F70C();
        if (result == 0) {
            goto out;
        }
        if (result == 1) {
            destination = D_801D1200;
            destination2 = destination + 256;
            source2 = destination + 360;
            source = destination + 104;
            end = destination + 168;
            do {
                *destination = *source;
                source++;
                destination++;
            } while (source != end);
            /* Swap the working pointer roles for the second restore copy. */
            source = destination2;
            destination = source2;
            end = destination + 64;
            do {
                *source = *destination;
                destination++;
                source++;
            } while (destination != end);
            i = 0;
            do {
                D_80185C9C[i][0] = 0;
                MainMenu_RefreshTradeInventory(i, 1);
                MainMenu_RebuildTradeInventoryRows(i);
                D_80185CC8[i] = 0;
                i++;
            } while (i < 2);
            func_8003CB7C();
            D_80185CCF = 0;
            D_80185CD0 = 0;
            D_80185CD1 = 0;
        } else {
            func_800611D0(*(volatile u8 *)&D_80185CCE);
            func_8003CB7C();
            D_80185CD0 = 0;
            D_80185CD1 = 0;
        }
        goto out;
    }

    if (D_80185CD0 != 0) {
        base = D_801D1200;
        backup_source2 = base + 256;
        backup_destination2 = base + 360;
        save_destination = base + 104;
        save_source = base;
        counts[0] = (u8 *)(base + 109);
        counts[1] = (u8 *)(base + 365);
        *(Block1024 *)save_destination = *(Block1024 *)save_source;
        save_source = backup_destination2;
        save_destination = backup_source2;
        *(Block1024 *)save_source = *(Block1024 *)save_destination;
        for (i = 0; i < 2; i++) {
            for (j = 0; j < D_80185C9C[i][0]; j++) {
                id = D_80185C9C[i][j + 1] - 1;
                from = counts[i] + id;
                if (*from != 0) {
                    *from = *from - 1;
                }
                to = counts[i ^ 1] + id;
                if (*to < CARD_CHEST_QUANTITY_MAX) {
                    *to = *to + 1;
                }
            }
        }
        func_8003FE14();
        D_80185CD1 = 1;
        goto out;
    }

    if (D_80185CCF != 0) {
        decision = 0;
        if ((D_8009B394[0] & 0x5000) != 0 ||
            (D_8009B394[1] & 0x5000) != 0) {
            func_80048658(6, 255, 0);
            if ((D_8009B394[0] & 0x1000) != 0 ||
                (D_8009B394[1] & 0x1000) != 0) {
                D_80185CCE = (D_80185CCE + 2) % 3;
            } else {
                D_80185CCE = (D_80185CCE + 4) % 3;
            }
            func_800611D0(*(volatile u8 *)&D_80185CCE);
            goto modal_return;
        }
        if ((D_8009B398[0] & 0x20) != 0 ||
            (D_8009B398[1] & 0x20) != 0) {
            other = ((D_8009B398[0] >> 5) ^ 1) & 1;
            func_80048658(8, 255, 0);
            func_80035B7C(D_800EB224);
            D_80185CC8[other] = 0;
            func_800610E0(other ^ 1);
            D_80185CCF = 0;
            goto modal_return;
        }
        if ((D_8009B398[0] & 0xC0) != 0 ||
            (D_8009B398[1] & 0xC0) != 0) {
            func_80048658(48, 255, 0);
            switch (D_80185CCE) {
            case 0:
                func_80035B7C(D_800EB224);
                D_80185CD0 = 1;
                break;
            case 1:
                func_80035B7C(D_800EB224);
                i = 1;
                clearFlags = D_80185CC8 + i;
                do {
                    *clearFlags = 0;
                    i--;
                    clearFlags--;
                } while (i >= 0);
                D_80185CCF = 0;
                break;
            case 2:
                decision = 1;
                break;
            }
        }
    modal_return:
        return decision;
    }

    if (D_80185C8C[0][0] != D_80185C8C[0][1]) {
        goto scroll0;
    }
    card0 = &D_801845FC[D_80185C8C[0][0] + D_80185CCA[0]];
    if (D_80185CC8[0] != 0) {
        if ((D_8009B398[0] & 0x20) != 0) {
            func_80048658(8, 255, 0);
            func_80035B7C(D_800EB224);
            D_80185CC8[0] = 0;
        }
        goto check_scroll0;
    }
    if ((D_8009B398[0] & 0x80) != 0) {
        if (D_80185CC8[1] != 0 &&
            D_80185C9C[0][0] + D_80185C9C[1][0] == 0) {
            { func_80048658(9, 255, 0); goto check_scroll0; }
        }
        func_80048658(48, 255, 0);
        func_800610E0(0);
        D_80185CC8[0] = 1;
        goto check_scroll0;
    }
    if ((D_8009B398[0] & 0x10) != 0) {
        if (card0->id != 0) {
            D_8009B246 = card0->id;
            D_8009B24B = 20;
            D_8009B254 = 2;
        }
        goto check_scroll0;
    }
    if ((D_8009B394[0] & 0x900) != 0) {
        func_80048658(47, 255, 0);
        if ((D_8009B394[0] & 0x100) != 0) {
            D_80185CCC[0] = (D_80185CCC[0] + 5) % 6;
        } else {
            D_80185CCC[0] = (D_80185CCC[0] + 7) % 6;
        }
        MainMenu_RefreshTradeInventory(0, 0);
    dirty0:
        dirty0++;
        goto check_scroll0;
    }
    if ((D_8009B398[0] & 0xA000) != 0) {
        if ((D_8009B398[0] & 0x8000) != 0) {
            flags &= ~1;
        } else {
            flags |= 1;
        }
        goto dirty0;
    }
    if ((D_8009B398[0] & 0x20) != 0) {
        if (D_80185C9C[0][0] != 0) {
            MainMenu_AdjustTradeCardCount(0, D_80185C9C[0][D_80185C9C[0][0]], 1);
            func_80048658(8, 255, 0);
            D_80185C9C[0][0]--;
            dirty0 = 1;
            goto check_scroll0;
        }
        if (D_80185C9C[1][0] != 0 || D_80185CC8[1] != 0) {
            { func_80048658(9, 255, 0); goto check_scroll0; }
        }
        goto leave;
    }
    if ((D_8009B398[0] & 0x40) != 0) {
        if (D_80185C9C[0][0] < 10) {
            if (card0->id == 0) {
                { func_80048658(9, 255, 0); goto check_scroll0; }
            }
            if (card0->count != 0) {
                func_80048658(7, 255, 0);
                D_80185C9C[0][0]++;
                D_80185C9C[0][D_80185C9C[0][0]] = card0->id;
                card0->count--;
                dirty0 = 1;
                goto check_scroll0;
            }
        }
        { func_80048658(9, 255, 0); goto check_scroll0; }
    }
    if ((D_8009B3A4[0] & 0xC) != 0) {
        if ((D_8009B3A4[0] & 4) != 0) {
            low = D_80185C8C[0][1] - 7;
            if (low < 0) { low = 0; }
            D_80185C8C[0][1] = low;
        } else {
            value = D_80185C8C[0][1] + 7;
            D_80185C8C[0][1] = value < maximum ? value : maximum;
        }
        if (D_80185C8C[0][0] == D_80185C8C[0][1]) {
            goto player1;
        }
        func_80048658(6, 255, 0);
        goto check_scroll0;
    }
    if ((D_8009B394[0] & 3) != 0) {
        if ((D_8009B394[0] & 1) != 0) {
            low = D_80185C8C[0][1] - 50;
            if (low < 0) { low = 0; }
            D_80185C8C[0][1] = low;
        } else {
            value = D_80185C8C[0][1] + 50;
            D_80185C8C[0][1] = value < maximum ? value : maximum;
        }
        if (D_80185C8C[0][0] != D_80185C8C[0][1]) {
            func_80048658(6, 255, 0);
        }
        D_80185C8C[0][0] = D_80185C8C[0][1];
        dirty0++;
        goto check_scroll0;
    }
    if ((D_8009B394[0] & 0x5000) != 0) {
        if ((D_8009B394[0] & 0x1000) != 0) {
            if (D_80185CCA[0] != 0) {
                func_80048658(6, 255, 0);
                D_80185CCA[0]--;
                cursor0++;
            } else if (D_80185C8C[0][1] != 0) {
                func_80048658(6, 255, 0);
                D_80185C8C[0][0] = --D_80185C8C[0][1];
                dirty0++;
            }
        } else if (D_80185CCA[0] < 6) {
            func_80048658(6, 255, 0);
            D_80185CCA[0]++;
            cursor0++;
        } else if (D_80185C8C[0][1] < maximum) {
            func_80048658(6, 255, 0);
            D_80185C8C[0][0] = ++D_80185C8C[0][1];
            dirty0++;
        }
    }
    goto check_scroll0;
check_scroll0:
    if (D_80185C8C[0][0] != D_80185C8C[0][1]) {
    scroll0:
        D_80185C8C[0][0] = D_80185C8C[0][0] < D_80185C8C[0][1] ?
            D_80185C8C[0][0] + 1 : D_80185C8C[0][0] - 1;
        dirty0++;
    }

player1:
    if (D_80185C8C[1][0] != D_80185C8C[1][1]) {
        goto scroll1;
    }
    card1 = &D_80185144[D_80185C8C[1][0] + D_80185CCA[1]];
    if (D_80185CC8[1] != 0) {
        if ((D_8009B398[1] & 0x20) != 0) {
            func_80048658(8, 255, 0);
            func_80035B7C(D_800EB224);
            D_80185CC8[1] = 0;
        }
        goto check_scroll1;
    }
    if ((D_8009B398[1] & 0x80) != 0) {
        if (D_80185CC8[0] != 0 &&
            D_80185C9C[0][0] + D_80185C9C[1][0] == 0) {
            { func_80048658(9, 255, 0); goto check_scroll1; }
        }
        func_80048658(48, 255, 0);
        func_800610E0(1);
        D_80185CC9 = 1;
        goto check_scroll1;
    }
    if ((D_8009B398[1] & 0x10) != 0) {
        if (card1->id != 0) {
            D_8009B246 = card1->id;
            D_8009B24B = 20;
            D_8009B254 = 2;
        }
        goto check_scroll1;
    }
    if ((D_8009B394[1] & 0x900) != 0) {
        func_80048658(47, 255, 0);
        if ((D_8009B394[1] & 0x100) != 0) {
            D_80185CCC[1] = (D_80185CCC[1] + 5) % 6;
        } else {
            D_80185CCC[1] = (D_80185CCC[1] + 7) % 6;
        }
        MainMenu_RefreshTradeInventory(1, 0);
    dirty1:
        dirty1++;
        goto check_scroll1;
    }
    if ((D_8009B398[1] & 0xA000) != 0) {
        if ((D_8009B398[1] & 0x8000) != 0) {
            flags &= ~2;
        } else {
            flags |= 2;
        }
        goto dirty1;
    }
    if ((D_8009B398[1] & 0x20) != 0) {
        if (D_80185C9C[1][0] != 0) {
            MainMenu_AdjustTradeCardCount(1, D_80185C9C[1][D_80185C9C[1][0]], 1);
            func_80048658(8, 255, 0);
            D_80185C9C[1][0]--;
            dirty1++;
            goto check_scroll1;
        }
        if (D_80185C9C[0][0] != 0 || D_80185CC8[0] != 0) {
            { func_80048658(9, 255, 0); goto check_scroll1; }
        }
    leave:
        func_80048658(8, 255, 0);
        return 1;
    }
    if ((D_8009B398[1] & 0x40) != 0) {
        if (D_80185C9C[1][0] < 10) {
            if (card1->id == 0) {
                { func_80048658(9, 255, 0); goto check_scroll1; }
            }
            if (card1->count != 0) {
                func_80048658(7, 255, 0);
                /* Use the increment result rather than reloading the count. */
                D_80185C9C[1][++D_80185C9C[1][0]] = card1->id;
                card1->count--;
                dirty1++;
                goto check_scroll1;
            }
        }
        { func_80048658(9, 255, 0); goto check_scroll1; }
    }
    if ((D_8009B3A4[1] & 0xC) != 0) {
        if ((D_8009B3A4[1] & 4) != 0) {
            low = D_80185C8C[1][1] - 7;
            if (low < 0) { low = 0; }
            D_80185C8C[1][1] = low;
        } else {
            bounded = maximum;
            if (D_80185C8C[1][1] + 7 < bounded) {
                bounded = D_80185C8C[1][1] + 7;
            }
            D_80185C8C[1][1] = bounded;
        }
        if (D_80185C8C[1][0] == D_80185C8C[1][1]) {
            goto update;
        }
        func_80048658(6, 255, 0);
        goto check_scroll1;
    }
    if ((D_8009B394[1] & 3) != 0) {
        if ((D_8009B394[1] & 1) != 0) {
            low = D_80185C8C[1][1] - 50;
            if (low < 0) { low = 0; }
            D_80185C8C[1][1] = low;
        } else {
            bounded = maximum;
            if (D_80185C8C[1][1] + 50 < bounded) {
                bounded = D_80185C8C[1][1] + 50;
            }
            D_80185C8C[1][1] = bounded;
        }
        if (D_80185C8C[1][0] != D_80185C8C[1][1]) {
            func_80048658(6, 255, 0);
        }
        D_80185C8C[1][0] = D_80185C8C[1][1];
        dirty1++;
        goto check_scroll1;
    }
    if ((D_8009B394[1] & 0x5000) != 0) {
        if ((D_8009B394[1] & 0x1000) != 0) {
            if (D_80185CCA[1] != 0) {
                func_80048658(6, 255, 0);
                D_80185CCA[1]--;
                cursor1++;
            } else if (D_80185C8C[1][1] != 0) {
                func_80048658(6, 255, 0);
                D_80185C8C[1][0] = --D_80185C8C[1][1];
                dirty1++;
            }
        } else if (D_80185CCA[1] < 6) {
            func_80048658(6, 255, 0);
            D_80185CCA[1]++;
            cursor1++;
        } else if (D_80185C8C[1][1] < maximum) {
            func_80048658(6, 255, 0);
            D_80185C8C[1][0] = ++D_80185C8C[1][1];
            dirty1++;
        }
    }
    goto check_scroll1;
check_scroll1:
    if (D_80185C8C[1][0] != D_80185C8C[1][1]) {
    scroll1:
        D_80185C8C[1][0] = D_80185C8C[1][0] < D_80185C8C[1][1] ?
            D_80185C8C[1][0] + 1 : D_80185C8C[1][0] - 1;
        dirty1++;
    }

update:
    if (cursor0 != 0) {
        D_801845EC->y = D_80185CCA[0] * 22 + 36;
    }
    if (cursor1 != 0) {
        D_801845F4->y = D_80185CCB * 22 + 36;
    }
    if (flags != previousFlags) {
        func_80048658(30, 255, 0);
        func_80040410(D_801845E0, flags + 4);
    }
    if (dirty0 != 0) {
        MainMenu_RebuildTradeInventoryRows(0);
    }
    if (dirty1 != 0) {
        MainMenu_RebuildTradeInventoryRows(1);
    }
    if (D_80185CC8[0] != 0 && D_80185CC8[1] != 0) {
        D_80185CCE = 0;
        func_80035B7C(D_800EB224);
        func_80035B7C(D_800EB224);
        func_800611D0(*(volatile u8 *)&D_80185CCE);
        D_80185CCF = 1;
    }
out:
    return 0;
}
