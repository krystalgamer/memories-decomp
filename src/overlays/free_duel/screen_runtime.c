#define GINPUT_PAD1_PRESSED_IS_VOLATILE
#define GINPUT_PAD1_HELD_IS_VOLATILE
#include "../../types.h"
#include "../../game/card_constants.h"
#include "../../game/display_object_layout.h"
#include "../../game/input.h"
#include "../../game/text_box_lifecycle.h"
#include "../../psyq/rand.h"
#include "../../game/sound.h"
#include "../../game/display_object_helpers.h"
#include "../../game/func_80039794.h"

extern u8 gFreeDuel_bScreenFlags;
extern u8 *gFreeDuel_pCursorWidget;
extern u8 gFreeDuel_abGridAvailable[];
extern u8 D_800EB15C[];
extern u8 D_8009B269;
extern u8 D_8009B26C;
extern u8 gFreeDuel_bReturnFlags;
extern s8 gFreeDuel_bCursorColumn;
extern s8 gFreeDuel_bCursorRow;
extern u8 D_8009B368;
extern s8 gFreeDuel_bTargetColumn;
extern s8 gFreeDuel_bTargetRow;
extern u16 D_801D0200[];
extern s32 D_8009B0CC;
extern u8 **FreeDuel_GetSparkleSlot(void);
extern u8 *FreeDuel_SpawnSparkle(void);
extern void FreeDuel_UpdateSparkle(void);
extern void func_80041D60(void *);
extern void func_80024DC8(s32, s32, s32, s32);
extern void func_80033C90(void);
extern void func_80035C38(s32, s32, s32, s32, s32, s32, s32);
extern void FreeDuel_PlaceCursor(void *, s32);
extern void FreeDuel_UpdateScrollbar(void);

void FreeDuel_UpdateCursorTween(void)
{
    u8 *widget = gFreeDuel_pCursorWidget;
    u8 **slot;
    u8 *sparkle;
    s32 tx;
    s32 ty;
    s32 sx;
    s32 d;
    s16 left;

    if ((gFreeDuel_bScreenFlags & 0x40) == 0) {
        if (gFreeDuel_bCursorColumn == gFreeDuel_bTargetColumn && gFreeDuel_bCursorRow == gFreeDuel_bTargetRow) {
            return;
        }
        gFreeDuel_bScreenFlags |= 0x40;
        *(u16 *)(widget + 0x60) = 8;
        func_800429D8(widget);

        d = gFreeDuel_bTargetColumn;
        tx = d * 56 + 20;
        d = *(s16 *)(widget + 0x30);
        d = tx - d;
        sx = (d << 8) / 8;
        d = gFreeDuel_bTargetRow;
        ty = d * 52 + 40;
        *(s16 *)(widget + 0x36) = sx;
        d = *(s16 *)(widget + 0x32);
        d = ty - d;
        *(s16 *)(widget + 0x38) = (d << 8) / 8;
    }

    func_80042A78(widget);
    left = *(u16 *)(widget + 0x60) - 1;
    *(u16 *)(widget + 0x60) = left;
    if (left == 0) {
        gFreeDuel_bCursorColumn = gFreeDuel_bTargetColumn;
        gFreeDuel_bCursorRow = gFreeDuel_bTargetRow;
        FreeDuel_PlaceCursor(widget, 1);
        gFreeDuel_bScreenFlags &= ~0x40;
        SD_SEPlayFull(47);
    } else {
        slot = FreeDuel_GetSparkleSlot();
        sparkle = FreeDuel_SpawnSparkle();
        if (sparkle != 0 && slot != 0) {
            *(u32 *)(sparkle + 0x30) = *(u32 *)(widget + 0x30);
            func_800428EC(sparkle, (s8)(widget[0x16] - 1));
            func_80041D60(sparkle);
            *(u32 *)(sparkle + 0x4C) = *(u32 *)(widget + 0x4C);
            sparkle[0x6C] = 1;
            *(u16 *)(sparkle + 8) |= 1;
            *slot = sparkle;
        }
    }
}

void FreeDuel_UpdateScreen(void)
{
    u8 *panel;
    u16 *entry;
    s32 index;

    if ((gFreeDuel_bScreenFlags & 0x20) != 0) {
        func_80039794();
        panel = D_800EB15C;
        if ((*(u16 *)(panel + 0x34) & 8) == 0) {
            gFreeDuel_bScreenFlags &= 0xDF;
            TextBox_Destroy(panel);
            *(u16 *)(gFreeDuel_pCursorWidget + 8) |=
                DISPLAY_OBJECT_FLAG_RENDERABLE;
            FreeDuel_PlaceCursor(gFreeDuel_pCursorWidget, 1);
        }
        return;
    }

    FreeDuel_UpdateCursorTween();
    FreeDuel_UpdateScrollbar();
    if ((gFreeDuel_bScreenFlags & 0x40) != 0) {
        return;
    }

    if ((gInput_wPad1Held & PAD_DIRECTION_MASK) != 0) {
        if ((gInput_wPad1Held & PAD_DIRECTION_RIGHT) != 0) {
            if (++gFreeDuel_bTargetColumn >= FREE_DUEL_GRID_COLUMN_COUNT) {
                gFreeDuel_bTargetColumn = FREE_DUEL_GRID_COLUMN_COUNT - 1;
            }
        }
        if ((gInput_wPad1Held & PAD_DIRECTION_LEFT) != 0) {
            if (--gFreeDuel_bTargetColumn < 0) {
                gFreeDuel_bTargetColumn = 0;
            }
        }
        if ((gInput_wPad1Held & PAD_DIRECTION_DOWN) != 0) {
            if (++gFreeDuel_bTargetRow >= FREE_DUEL_GRID_ROW_COUNT) {
                gFreeDuel_bTargetRow = FREE_DUEL_GRID_ROW_COUNT - 1;
            }
        }
        if ((gInput_wPad1Held & PAD_DIRECTION_UP) != 0) {
            if (--gFreeDuel_bTargetRow <= 0) {
                gFreeDuel_bTargetRow = 0;
            }
        }
    } else {
        if ((gInput_wPad1Pressed & PAD_BUTTON_CANCEL) != 0) {
            SD_SEPlayFull(8);
            D_8009B26C = 8;
            return;
        }
        if ((gInput_wPad1Pressed & PAD_BUTTON_CONFIRM_MASK) == 0) {
            return;
        }
        if (gFreeDuel_abGridAvailable[
                gFreeDuel_bCursorRow * FREE_DUEL_GRID_COLUMN_COUNT +
                gFreeDuel_bCursorColumn] == 0) {
            return;
        }
        if ((gFreeDuel_bCursorColumn | gFreeDuel_bCursorRow) == 0) {
            func_80033C90();
            D_8009B269 = 6;
            gFreeDuel_bReturnFlags = 0x40;
            SD_SEPlayFull(0x30);
            return;
        }
        entry = D_801D0200;
        for (index = 0; index < DECK_SIZE; index++) {
            if (*entry == 0) {
                SD_SEPlayFull(9);
                func_80035C38(1, 8, 0x30, 0x6C, 0xE0, 0x10, 0x1028);
                gFreeDuel_bScreenFlags |= 0x20;
                return;
            }
            entry++;
        }
        SD_SEPlayFull(0x30);
        gFreeDuel_bReturnFlags = 0x80;
        func_80024DC8(
            -1,
            gFreeDuel_bCursorRow * FREE_DUEL_GRID_COLUMN_COUNT +
                gFreeDuel_bCursorColumn,
            0x6000, 0x6000);
        D_8009B368 = 6;
        D_8009B26C = 3;
    }
}

void FreeDuel_Entry(void)
{
    s32 phase;

    rand();
    FreeDuel_UpdateScreen();
    phase = D_8009B0CC & 0x7F;
    if (phase < 0x10) {
        if (phase >= 8) {
            phase = 0xF - phase;
        }
        *(s16 *)(gFreeDuel_pCursorWidget + 0x46) = phase * 48 + 0x1000;
        *(s16 *)(gFreeDuel_pCursorWidget + 0x44) = phase * 48 + 0x1000;
    }
    FreeDuel_UpdateSparkle();
}
