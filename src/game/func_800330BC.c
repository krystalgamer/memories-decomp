#define GINPUT_PAD1_HELD_IS_VOLATILE
#include "../types.h"
#include "card_list_text_boxes.h"
#include "card_list_sort.h"
#include "input.h"
#include "sound.h"

/* Card-list cursor and page input handler.

   Levers that mattered here:
   - The retry path is a backward goto, not a for(;;)/continue loop: with a
     real loop construct loop.c hoists the constant 7 into a callee-saved
     register instead of rematerialising it at each of its three uses.
   - The shared sub-row commit tail is written once and reached by
     "goto finish" from the second branch, which reproduces the retail block
     order (the tail sits between the L1/R1 block and the L2/R2 block).
     Duplicating it in both branches leaves the two copies unmerged.
   - "*(s16 *)(p + 0x2D40) - 8" is respelled at every use; a single local
     would collapse the reloads the target makes at each join point.
   - The D_80090DD8 lookup keeps func_80033CC4's statement-by-statement
     spelling so the base and slot pseudos land in the same registers.
*/

extern u8 D_80090DD8[];
extern volatile u16 gInput_wPad1Repeat;

s32 func_800330BC(u8 *p)
{
    s32 row;
    s32 sel;
    s32 icon;
    s32 slot;
    u8 *entry;

    row = *(s16 *)(p + 0x2D3C) | *(s8 *)(p + 0x2D48);
    if (row != 0) {
        row = (*(s16 *)(p + 0x2D3C) + *(s8 *)(p + 0x2D48) + 1) * 152 /
              *(s16 *)(p + 0x2D42);
    }
    *(s16 *)(*(u8 **)(p + 0x2D38) + 0x32) = row + 0x29;

top:
    if (*(s16 *)(p + 0x2D3C) != *(s16 *)(p + 0x2D3E)) {
        if (*(s16 *)(p + 0x2D3E) < *(s16 *)(p + 0x2D3C)) {
            *(u16 *)(p + 0x2D3C) = *(u16 *)(p + 0x2D3C) - 1;
        } else {
            *(u16 *)(p + 0x2D3C) = *(u16 *)(p + 0x2D3C) + 1;
        }
        func_80031E04(p, 8);
        return 1;
    }

    if ((gInput_wPad1Held & PAD_BUTTON_L1_R1_MASK) != 0) {
        sel = -1;
        row = *(s16 *)(p + 0x2D3C);
        if ((gInput_wPad1Held & PAD_BUTTON_R1) != 0) {
            if (row == *(s16 *)(p + 0x2D40) - 8 &&
                *(s8 *)(p + 0x2D48) != 7) {
                sel = 7;
            }
            row += 8;
            if (*(s16 *)(p + 0x2D40) - 8 < row) {
                row = *(s16 *)(p + 0x2D40) - 8;
            }
        } else {
            if (row == 0 && *(s8 *)(p + 0x2D48) != 0) {
                sel = 0;
            }
            row -= 8;
        }
        if (row < 0) {
            row = 0;
        }
        *(s16 *)(p + 0x2D3E) = row;
        if (*(s16 *)(p + 0x2D3C) != row) {
            SD_SEPlayFull(6);
            goto top;
        }
    finish:
        if (sel >= 0) {
            *(s8 *)(p + 0x2D48) = sel;
            *(s16 *)(*(u8 **)(p + 0x2D34) + 0x32) = sel * 22 + 0x2A;
            SD_SEPlayFull(6);
        }
        return 1;
    }

    if ((gInput_wPad1Repeat & PAD_BUTTON_TRIGGER_MASK) != 0) {
        sel = -1;
        row = *(s16 *)(p + 0x2D3C);
        if ((gInput_wPad1Repeat & PAD_BUTTON_R2) != 0) {
            if (row == *(s16 *)(p + 0x2D40) - 8 &&
                *(s8 *)(p + 0x2D48) != 7) {
                sel = 7;
            }
            row += 0x32;
            if (*(s16 *)(p + 0x2D40) - 8 < row) {
                row = *(s16 *)(p + 0x2D40) - 8;
            }
        } else {
            if (row == 0 && *(s8 *)(p + 0x2D48) != 0) {
                sel = 0;
            }
            row -= 0x32;
            if (row < 0) {
                row = 0;
            }
        }
        if (*(s16 *)(p + 0x2D3C) != row) {
            SD_SEPlayFull(6);
            *(s16 *)(p + 0x2D3E) = row;
            *(s16 *)(p + 0x2D3C) = row;
            func_80031E04(p, 8);
            return 1;
        }
        goto finish;
    }

    if ((gInput_wPad1Repeat & PAD_DIRECTION_VERTICAL_MASK) != 0) {
        row = *(s16 *)(p + 0x2D3C);
        if ((gInput_wPad1Repeat & PAD_DIRECTION_DOWN) != 0) {
            *(s8 *)(p + 0x2D48) += 1;
            if (*(s8 *)(p + 0x2D48) >= 8) {
                row += 1;
                *(s8 *)(p + 0x2D48) = 7;
            }
        } else {
            *(s8 *)(p + 0x2D48) -= 1;
            if (*(s8 *)(p + 0x2D48) < 0) {
                row -= 1;
                *(s8 *)(p + 0x2D48) = 0;
            }
        }
        *(s16 *)(*(u8 **)(p + 0x2D34) + 0x32) =
            *(s8 *)(p + 0x2D48) * 22 + 0x2A;
        if (*(s16 *)(p + 0x2D40) - 8 < row) {
            return 1;
        }
        if (row < 0) {
            return 1;
        }
        SD_SEPlayFull(6);
        *(s16 *)(p + 0x2D3E) = row;
        if (*(s16 *)(p + 0x2D3C) != row) {
            goto top;
        }
        return 1;
    }

    if ((gInput_wPad1Repeat & (PAD_BUTTON_SELECT | PAD_BUTTON_START)) !=
        0) {
        if ((gInput_wPad1Repeat & PAD_BUTTON_START) != 0) {
            *(s8 *)(p + 0x2D46) += 1;
            if (*(s8 *)(p + 0x2D46) >= 7) {
                *(s8 *)(p + 0x2D46) = 0;
            }
        } else {
            *(s8 *)(p + 0x2D46) -= 1;
            if (*(s8 *)(p + 0x2D46) < 0) {
                *(s8 *)(p + 0x2D46) = 6;
            }
        }
        SD_SEPlayFull(0x2F);
        icon = p[0x2D47];
        slot = *(s8 *)(p + 0x2D46);
        icon = icon << 4;
        slot = slot << 1;
        entry = D_80090DD8 + slot;
        icon = icon + (s32)entry;
        p[0x2D45] = *(u8 *)(icon + 1) & 0xF;
        func_80032C48(p);
        return 1;
    }

    return 0;
}
