#define GINPUT_PAD1_REPEAT_SIZED_VOLATILE
#define GINPUT_PAD1_PRESSED_SIZED_VOLATILE
#define GINPUT_PAD1_HELD_SIZED_VOLATILE
#include "../../types.h"
#include "../../game/graphics_frame.h"
#include "../../game/display_object_core.h"
#include "../../game/display_object_helpers.h"
#include "../../game/main_services.h"
#include "../../game/two_player_save_setup.h"
#include "../../game/save_data.h"
#include "../../ygo_types.h"
#include "../../psyq/qsort.h"
#include "../../game/card_constants.h"
#define MAIN_MENU_TRADE_STATE_BUFFER
#include "trade_helpers.h"
#include "../../game/display_object_config.h"
#include "../../game/data_transfer_request.h"
#include "../../game/duel_card_viewer.h"
#include "../../game/duel_effect.h"
#include "../../game/sound.h"
#include "../../game/input.h"
#include "../../game/text_box_lifecycle.h"
#include "../../game/func_80061008.h"
#include "../../game/func_800610E0.h"
#include "../../game/func_800611D0.h"
#include "../../unmatched.h"

void MainMenu_InitTradeScreen(void)
{
    u8 *object;
    DisplayObject *entry;
    s32 i;

    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    D_801845DC = (MainMenuWidget *)object;
    if (object != 0) {
        DisplayObject_ConfigureSpriteAtPosition(object, 0, 0, 0, 4, 0xB, 0xC, 0x208);
        D_801845DC->flags |=
            DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
            DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_SetDepthOffset(D_801845DC, -2);
    }

    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    D_801845E0 = (MainMenuWidget *)object;
    if (object != 0) {
        DisplayObject_ConfigureSpriteAtPosition(object, 0, -3, 0, 4, 4, 0xC, 0x208);
        D_801845E0->flags |=
            DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
            DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_SetDepthOffset(D_801845E0, -1);
    }

    func_80061008(0, 0x25, 0xA0, 0x25);

    for (i = 0; i < 2; i++) {
        entry = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        if (entry != 0) {
            DisplayObject_ConfigureSpriteAtPosition(entry, i * 0xA0 + 0x1E, 0x24, 0, 4, 8, 0xC, 0x208);
            entry->flags |=
                DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
            DisplayObject_SetDepthOffset(entry, 0);
            entry->field_60 = -2;
            D_801845EC[i].object = (u8 *)entry;
        } else {
            D_801845EC[i].object = 0;
        }
        D_801845EC[i].unk4 = 0;
        D_80185C8C[i].target = 0;
        D_80185C8C[i].current = 0;
        D_80185C9C[i][0] = 0;
        D_80185CC8[i] = 0;
        D_80185CCA[i] = 0;
        D_80185CCC[i] = 0;
        MainMenu_RefreshTradeInventory(i, 1);
        MainMenu_RebuildTradeInventoryRows(i);
    }

    D_80185CCE = 0;
    D_80185CCF = 0;
    D_80185CD0 = 0;
    D_80185CD1 = 0;
    D_800E9DB0[1] = MainMenu_DrawTradeOffersAndHighlights;
    D_8009B0C0 = 1;
}

extern CardCountEntry D_80185144[];
extern u8 D_80185CCB;
extern u16 D_80185C8C_words[2][2] asm("D_80185C8C");

#define D_80185C8C D_80185C8C_words

/*
 * Exact pure-C match under gcc_2_8_1_g0_split. Natural restore-pointer roles
 * recover a1/a2, a path-exclusive typed carrier recovers s2, and one
 * single-iteration dirty-counter loop gives GCC the weight needed to allocate
 * that counter to s3 without a hard-register binding.
 */
s32 MainMenu_UpdateTradeScreen(void)
{
    s32 bounded;
    s32 low;
    s32 decision;
    s32 dirty0;
    MainMenuTradeDirtyCarrier dirty1;
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
    MainMenuTradeBlock16 *base;
    MainMenuTradeBlock16 *source;
    MainMenuTradeBlock16 *save_source;
    MainMenuTradeBlock16 *destination;
    MainMenuTradeBlock16 *save_destination;
    MainMenuTradeBlock16 *end;
    MainMenuTradeBlock16 *source2;
    MainMenuTradeBlock16 *backup_source2;
    MainMenuTradeBlock16 *destination2;
    MainMenuTradeBlock16 *backup_destination2;
    CardCountEntry *card0;
    CardCountEntry *card1;

    dirty0 = 0;
    dirty1.dirty = dirty0;
    cursor0 = 1;
    cursor1 = cursor0;
    value = D_801845E0->frame - 4;
    previousFlags = value;
    flags = value;
    maximum = 715;

    if (D_80185CD1 != 0) {
        result = MemCardDialog_Poll();
        if (result == 0) {
            goto out;
        }
        if (result == 1) {
            destination = (MainMenuTradeBlock16 *)D_801D1200;
            source2 = destination + 256;
            destination2 = destination + 360;
            source = destination + 104;
            end = destination + 168;
            do {
                *destination = *source;
                source++;
                destination++;
            } while (source != end);
            source = source2;
            destination = destination2;
            end = destination + 64;
            do {
                *source = *destination;
                destination++;
                source++;
            } while (destination != end);
            i = 0;
            dirty1.clearBase = D_80185CC8;
            do {
                D_80185C9C[i][0] = 0;
                MainMenu_RefreshTradeInventory(i, 1);
                MainMenu_RebuildTradeInventoryRows(i);
                *(u8 *)((s32)i + (s32)dirty1.clearBase) = 0;
                i++;
            } while (i < 2);
            Input_ResetPads();
            D_80185CCF = 0;
            D_80185CD0 = 0;
            D_80185CD1 = 0;
        } else {
            func_800611D0(*(volatile u8 *)&D_80185CCE);
            Input_ResetPads();
            D_80185CD0 = 0;
            D_80185CD1 = 0;
        }
        goto out;
    }

    if (D_80185CD0 != 0) {
        base = (MainMenuTradeBlock16 *)D_801D1200;
        backup_source2 = base + 256;
        backup_destination2 = base + 360;
        save_destination = base + 104;
        save_source = base;
        counts[0] = (u8 *)(base + 109);
        counts[1] = (u8 *)(base + 365);
        *(MainMenuTradeBlock1024 *)save_destination =
            *(MainMenuTradeBlock1024 *)save_source;
        save_source = backup_destination2;
        save_destination = backup_source2;
        *(MainMenuTradeBlock1024 *)save_source =
            *(MainMenuTradeBlock1024 *)save_destination;
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
        SaveData_RequestTradeWrite();
        D_80185CD1 = 1;
        goto out;
    }

    if (D_80185CCF != 0) {
        decision = 0;
        if ((gInput_wPad1Repeat[0] & 0x5000) != 0 ||
            (gInput_wPad1Repeat[1] & 0x5000) != 0) {
            SD_SEPlay(6, 255, 0);
            if ((gInput_wPad1Repeat[0] & 0x1000) != 0 ||
                (gInput_wPad1Repeat[1] & 0x1000) != 0) {
                D_80185CCE = (D_80185CCE + 2) % 3;
            } else {
                D_80185CCE = (D_80185CCE + 4) % 3;
            }
            func_800611D0(*(volatile u8 *)&D_80185CCE);
            goto modal_return;
        }
        if ((gInput_wPad1Pressed[0] & 0x20) != 0 ||
            (gInput_wPad1Pressed[1] & 0x20) != 0) {
            other = ((gInput_wPad1Pressed[0] >> 5) ^ 1) & 1;
            SD_SEPlay(8, 255, 0);
            TextBox_Destroy((DuelEffectChannel *)D_800EB224);
            D_80185CC8[other] = 0;
            func_800610E0(other ^ 1);
            D_80185CCF = 0;
            goto modal_return;
        }
        if ((gInput_wPad1Pressed[0] & 0xC0) != 0 ||
            (gInput_wPad1Pressed[1] & 0xC0) != 0) {
            SD_SEPlay(48, 255, 0);
            switch (D_80185CCE) {
            case 0:
                TextBox_Destroy((DuelEffectChannel *)D_800EB224);
                D_80185CD0 = 1;
                break;
            case 1:
                TextBox_Destroy((DuelEffectChannel *)D_800EB224);
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
    card0 = &D_801845FC[0][D_80185C8C[0][0] + D_80185CCA[0]];
    if (D_80185CC8[0] != 0) {
        if ((gInput_wPad1Pressed[0] & 0x20) != 0) {
            SD_SEPlay(8, 255, 0);
            TextBox_Destroy((DuelEffectChannel *)D_800EB224);
            D_80185CC8[0] = 0;
        }
        goto check_scroll0;
    }
    if ((gInput_wPad1Pressed[0] & 0x80) != 0) {
        if (D_80185CC8[1] != 0 &&
            D_80185C9C[0][0] + D_80185C9C[1][0] == 0) {
            { SD_SEPlay(9, 255, 0); goto check_scroll0; }
        }
        SD_SEPlay(48, 255, 0);
        func_800610E0(0);
        D_80185CC8[0] = 1;
        goto check_scroll0;
    }
    if ((gInput_wPad1Pressed[0] & 0x10) != 0) {
        if (card0->id != 0) {
            gDuel_wViewerCardID = card0->id;
            gDuel_bCardViewerYOffset = 20;
            gDuel_bEffectState = 2;
        }
        goto check_scroll0;
    }
    if ((gInput_wPad1Repeat[0] & 0x900) != 0) {
        SD_SEPlay(47, 255, 0);
        if ((gInput_wPad1Repeat[0] & 0x100) != 0) {
            D_80185CCC[0] = (D_80185CCC[0] + 5) % 6;
        } else {
            D_80185CCC[0] = (D_80185CCC[0] + 7) % 6;
        }
        MainMenu_RefreshTradeInventory(0, 0);
    dirty0:
        do {
            dirty0++;
        } while (0);
        goto check_scroll0;
    }
    if ((gInput_wPad1Pressed[0] & 0xA000) != 0) {
        if ((gInput_wPad1Pressed[0] & 0x8000) != 0) {
            flags &= ~1;
        } else {
            flags |= 1;
        }
        goto dirty0;
    }
    if ((gInput_wPad1Pressed[0] & 0x20) != 0) {
        if (D_80185C9C[0][0] != 0) {
            MainMenu_AdjustTradeCardCount(0, D_80185C9C[0][D_80185C9C[0][0]], 1);
            SD_SEPlay(8, 255, 0);
            D_80185C9C[0][0]--;
            dirty0 = 1;
            goto check_scroll0;
        }
        if (D_80185C9C[1][0] != 0 || D_80185CC8[1] != 0) {
            { SD_SEPlay(9, 255, 0); goto check_scroll0; }
        }
        goto leave;
    }
    if ((gInput_wPad1Pressed[0] & 0x40) != 0) {
        if (D_80185C9C[0][0] < 10) {
            if (card0->id == 0) {
                { SD_SEPlay(9, 255, 0); goto check_scroll0; }
            }
            if (card0->count != 0) {
                SD_SEPlay(7, 255, 0);
                D_80185C9C[0][0]++;
                D_80185C9C[0][D_80185C9C[0][0]] = card0->id;
                card0->count--;
                dirty0 = 1;
                goto check_scroll0;
            }
        }
        { SD_SEPlay(9, 255, 0); goto check_scroll0; }
    }
    if ((gInput_wPad1Held[0] & 0xC) != 0) {
        if ((gInput_wPad1Held[0] & 4) != 0) {
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
        SD_SEPlay(6, 255, 0);
        goto check_scroll0;
    }
    if ((gInput_wPad1Repeat[0] & 3) != 0) {
        if ((gInput_wPad1Repeat[0] & 1) != 0) {
            low = D_80185C8C[0][1] - 50;
            if (low < 0) { low = 0; }
            D_80185C8C[0][1] = low;
        } else {
            value = D_80185C8C[0][1] + 50;
            D_80185C8C[0][1] = value < maximum ? value : maximum;
        }
        if (D_80185C8C[0][0] != D_80185C8C[0][1]) {
            SD_SEPlay(6, 255, 0);
        }
        D_80185C8C[0][0] = D_80185C8C[0][1];
        dirty0++;
        goto check_scroll0;
    }
    if ((gInput_wPad1Repeat[0] & 0x5000) != 0) {
        if ((gInput_wPad1Repeat[0] & 0x1000) != 0) {
            if (D_80185CCA[0] != 0) {
                SD_SEPlay(6, 255, 0);
                D_80185CCA[0]--;
                cursor0++;
            } else if (D_80185C8C[0][1] != 0) {
                SD_SEPlay(6, 255, 0);
                D_80185C8C[0][0] = --D_80185C8C[0][1];
                dirty0++;
            }
        } else if (D_80185CCA[0] < 6) {
            SD_SEPlay(6, 255, 0);
            D_80185CCA[0]++;
            cursor0++;
        } else if (D_80185C8C[0][1] < maximum) {
            SD_SEPlay(6, 255, 0);
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
        if ((gInput_wPad1Pressed[1] & 0x20) != 0) {
            SD_SEPlay(8, 255, 0);
            TextBox_Destroy((DuelEffectChannel *)D_800EB224);
            D_80185CC8[1] = 0;
        }
        goto check_scroll1;
    }
    if ((gInput_wPad1Pressed[1] & 0x80) != 0) {
        if (D_80185CC8[0] != 0 &&
            D_80185C9C[0][0] + D_80185C9C[1][0] == 0) {
            { SD_SEPlay(9, 255, 0); goto check_scroll1; }
        }
        SD_SEPlay(48, 255, 0);
        func_800610E0(1);
        D_80185CC9 = 1;
        goto check_scroll1;
    }
    if ((gInput_wPad1Pressed[1] & 0x10) != 0) {
        if (card1->id != 0) {
            gDuel_wViewerCardID = card1->id;
            gDuel_bCardViewerYOffset = 20;
            gDuel_bEffectState = 2;
        }
        goto check_scroll1;
    }
    if ((gInput_wPad1Repeat[1] & 0x900) != 0) {
        SD_SEPlay(47, 255, 0);
        if ((gInput_wPad1Repeat[1] & 0x100) != 0) {
            D_80185CCC[1] = (D_80185CCC[1] + 5) % 6;
        } else {
            D_80185CCC[1] = (D_80185CCC[1] + 7) % 6;
        }
        MainMenu_RefreshTradeInventory(1, 0);
    dirty1_label:
        dirty1.dirty++;
        goto check_scroll1;
    }
    if ((gInput_wPad1Pressed[1] & 0xA000) != 0) {
        if ((gInput_wPad1Pressed[1] & 0x8000) != 0) {
            flags &= ~2;
        } else {
            flags |= 2;
        }
        goto dirty1_label;
    }
    if ((gInput_wPad1Pressed[1] & 0x20) != 0) {
        if (D_80185C9C[1][0] != 0) {
            MainMenu_AdjustTradeCardCount(1, D_80185C9C[1][D_80185C9C[1][0]], 1);
            SD_SEPlay(8, 255, 0);
            D_80185C9C[1][0]--;
            dirty1.dirty++;
            goto check_scroll1;
        }
        if (D_80185C9C[0][0] != 0 || D_80185CC8[0] != 0) {
            { SD_SEPlay(9, 255, 0); goto check_scroll1; }
        }
    leave:
        SD_SEPlay(8, 255, 0);
        return 1;
    }
    if ((gInput_wPad1Pressed[1] & 0x40) != 0) {
        if (D_80185C9C[1][0] < 10) {
            if (card1->id == 0) {
                { SD_SEPlay(9, 255, 0); goto check_scroll1; }
            }
            if (card1->count != 0) {
                SD_SEPlay(7, 255, 0);
                /* Use the increment result rather than reloading the count. */
                D_80185C9C[1][++D_80185C9C[1][0]] = card1->id;
                card1->count--;
                dirty1.dirty++;
                goto check_scroll1;
            }
        }
        { SD_SEPlay(9, 255, 0); goto check_scroll1; }
    }
    if ((gInput_wPad1Held[1] & 0xC) != 0) {
        if ((gInput_wPad1Held[1] & 4) != 0) {
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
        SD_SEPlay(6, 255, 0);
        goto check_scroll1;
    }
    if ((gInput_wPad1Repeat[1] & 3) != 0) {
        if ((gInput_wPad1Repeat[1] & 1) != 0) {
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
            SD_SEPlay(6, 255, 0);
        }
        D_80185C8C[1][0] = D_80185C8C[1][1];
        dirty1.dirty++;
        goto check_scroll1;
    }
    if ((gInput_wPad1Repeat[1] & 0x5000) != 0) {
        if ((gInput_wPad1Repeat[1] & 0x1000) != 0) {
            if (D_80185CCA[1] != 0) {
                SD_SEPlay(6, 255, 0);
                D_80185CCA[1]--;
                cursor1++;
            } else if (D_80185C8C[1][1] != 0) {
                SD_SEPlay(6, 255, 0);
                D_80185C8C[1][0] = --D_80185C8C[1][1];
                dirty1.dirty++;
            }
        } else if (D_80185CCA[1] < 6) {
            SD_SEPlay(6, 255, 0);
            D_80185CCA[1]++;
            cursor1++;
        } else if (D_80185C8C[1][1] < maximum) {
            SD_SEPlay(6, 255, 0);
            D_80185C8C[1][0] = ++D_80185C8C[1][1];
            dirty1.dirty++;
        }
    }
    goto check_scroll1;
check_scroll1:
    if (D_80185C8C[1][0] != D_80185C8C[1][1]) {
    scroll1:
        D_80185C8C[1][0] = D_80185C8C[1][0] < D_80185C8C[1][1] ?
            D_80185C8C[1][0] + 1 : D_80185C8C[1][0] - 1;
        dirty1.dirty++;
    }

update:
    if (cursor0 != 0) {
        ((MainMenuWidget *)D_801845EC[0].object)->y = D_80185CCA[0] * 22 + 36;
    }
    if (cursor1 != 0) {
        ((MainMenuWidget *)D_801845EC[1].object)->y = D_80185CCB * 22 + 36;
    }
    if (flags != previousFlags) {
        SD_SEPlay(30, 255, 0);
        DisplayObject_SetResourceVariant(D_801845E0, flags + 4);
    }
    if (dirty0 != 0) {
        MainMenu_RebuildTradeInventoryRows(0);
    }
    if (dirty1.dirty != 0) {
        MainMenu_RebuildTradeInventoryRows(1);
    }
    if (D_80185CC8[0] != 0 && D_80185CC8[1] != 0) {
        D_80185CCE = 0;
        TextBox_Destroy((DuelEffectChannel *)D_800EB224);
        TextBox_Destroy((DuelEffectChannel *)D_800EB224);
        func_800611D0(*(volatile u8 *)&D_80185CCE);
        D_80185CCF = 1;
    }
out:
    return 0;
}
