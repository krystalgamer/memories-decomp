#include "../../types.h"
#include "../../game/card_constants.h"

extern u8 gFreeDuel_bScreenFlags;
extern u8 *gFreeDuel_pCursorWidget;
extern u8 gFreeDuel_abGridAvailable[];
extern u8 D_800EB15C[];
extern volatile u16 D_8009B398;
extern volatile u16 D_8009B3A4;
extern u8 D_8009B269;
extern u8 D_8009B26C;
extern u8 gFreeDuel_bReturnFlags;
extern s8 gFreeDuel_bCursorColumn;
extern s8 gFreeDuel_bCursorRow;
extern u8 D_8009B368;
extern s8 gFreeDuel_bTargetColumn;
extern s8 gFreeDuel_bTargetRow;
extern u16 D_801D0200[];
extern void func_80024DC8(s32, s32, s32, s32);
extern void func_80033C90(void);
extern void TextBox_Destroy(u8 *);
extern void func_80035C38(s32, s32, s32, s32, s32, s32, s32);
extern void func_80039794(void);
extern void SD_SEPlayFull(s32);
extern void FreeDuel_PlaceCursor(void *, s32);
extern void FreeDuel_UpdateCursorTween(void);
extern void FreeDuel_UpdateScrollbar(void);

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
            *(u16 *)(gFreeDuel_pCursorWidget + 8) |= 0x40;
            FreeDuel_PlaceCursor(gFreeDuel_pCursorWidget, 1);
        }
        return;
    }

    FreeDuel_UpdateCursorTween();
    FreeDuel_UpdateScrollbar();
    if ((gFreeDuel_bScreenFlags & 0x40) != 0) {
        return;
    }

    if ((D_8009B3A4 & 0xF000) != 0) {
        if ((D_8009B3A4 & 0x2000) != 0) {
            if (++gFreeDuel_bTargetColumn >= 5) {
                gFreeDuel_bTargetColumn = 4;
            }
        }
        if ((D_8009B3A4 & 0x8000) != 0) {
            if (--gFreeDuel_bTargetColumn < 0) {
                gFreeDuel_bTargetColumn = 0;
            }
        }
        if ((D_8009B3A4 & 0x4000) != 0) {
            if (++gFreeDuel_bTargetRow >= 8) {
                gFreeDuel_bTargetRow = 7;
            }
        }
        if ((D_8009B3A4 & 0x1000) != 0) {
            if (--gFreeDuel_bTargetRow <= 0) {
                gFreeDuel_bTargetRow = 0;
            }
        }
    } else {
        if ((D_8009B398 & 0x20) != 0) {
            SD_SEPlayFull(8);
            D_8009B26C = 8;
            return;
        }
        if ((D_8009B398 & 0xC0) == 0) {
            return;
        }
        if (gFreeDuel_abGridAvailable[gFreeDuel_bCursorRow * 5 + gFreeDuel_bCursorColumn] == 0) {
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
        func_80024DC8(-1, gFreeDuel_bCursorRow * 5 + gFreeDuel_bCursorColumn, 0x6000, 0x6000);
        D_8009B368 = 6;
        D_8009B26C = 3;
    }
}
