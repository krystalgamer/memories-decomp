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
extern s8 D_8009B366;
extern s8 D_8009B367;
extern u8 D_8009B368;
extern s8 D_8009B36C;
extern s8 D_8009B36D;
extern u16 D_801D0200[];
extern void func_80024DC8(s32, s32, s32, s32);
extern void func_80033C90(void);
extern void func_80035B7C(void *);
extern void func_80035C38(s32, s32, s32, s32, s32, s32, s32);
extern void func_80039794(void);
extern void func_8003FEE0(s32);
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
            func_80035B7C(panel);
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
            if (++D_8009B36C >= 5) {
                D_8009B36C = 4;
            }
        }
        if ((D_8009B3A4 & 0x8000) != 0) {
            if (--D_8009B36C < 0) {
                D_8009B36C = 0;
            }
        }
        if ((D_8009B3A4 & 0x4000) != 0) {
            if (++D_8009B36D >= 8) {
                D_8009B36D = 7;
            }
        }
        if ((D_8009B3A4 & 0x1000) != 0) {
            if (--D_8009B36D <= 0) {
                D_8009B36D = 0;
            }
        }
    } else {
        if ((D_8009B398 & 0x20) != 0) {
            func_8003FEE0(8);
            D_8009B26C = 8;
            return;
        }
        if ((D_8009B398 & 0xC0) == 0) {
            return;
        }
        if (gFreeDuel_abGridAvailable[D_8009B367 * 5 + D_8009B366] == 0) {
            return;
        }
        if ((D_8009B366 | D_8009B367) == 0) {
            func_80033C90();
            D_8009B269 = 6;
            gFreeDuel_bReturnFlags = 0x40;
            func_8003FEE0(0x30);
            return;
        }
        entry = D_801D0200;
        for (index = 0; index < DECK_SIZE; index++) {
            if (*entry == 0) {
                func_8003FEE0(9);
                func_80035C38(1, 8, 0x30, 0x6C, 0xE0, 0x10, 0x1028);
                gFreeDuel_bScreenFlags |= 0x20;
                return;
            }
            entry++;
        }
        func_8003FEE0(0x30);
        gFreeDuel_bReturnFlags = 0x80;
        func_80024DC8(-1, D_8009B367 * 5 + D_8009B366, 0x6000, 0x6000);
        D_8009B368 = 6;
        D_8009B26C = 3;
    }
}
