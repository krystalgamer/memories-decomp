/* Reclassified from matching_c (#3859). This was src/game/func_800330BC.c,
 * byte-exact only under gcc_2_8_1_cc_g8_as_g0_split, whose compiler and
 * assembler disagree about small data (GCC -G8, MASPSX -G0). Under
 * gcc_2_8_1_g0, a single threshold, it is 273 of 273 instructions with 6
 * differing, opcode distance 0. The source below is the match, unchanged
 * apart from its include paths. */
#define GINPUT_PAD1_HELD_IS_VOLATILE
#define GINPUT_PAD1_REPEAT_IS_VOLATILE
#include "../types.h"
#include "../unmatched.h"
#include "../game/card_type_icon_table.h"
#include "../game/card_list_text_boxes.h"
#include "../game/card_list_sort.h"
#include "../game/input.h"
#include "../game/sound.h"

/* Card-list cursor and page input handler.

   Levers that mattered here:
   - The retry path is a backward goto, not a for(;;)/continue loop: with a
     real loop construct loop.c hoists the constant 7 into a callee-saved
     register instead of rematerialising it at each of its three uses.
   - The shared sub-row commit tail is written once and reached by
     "goto finish" from the second branch, which reproduces the retail block
     order (the tail sits between the L1/R1 block and the L2/R2 block).
     Duplicating it in both branches leaves the two copies unmerged.
   - "list->row_count - 8" is respelled at every use; a single local
     would collapse the reloads the target makes at each join point.
   - The D_80090DD8 lookup keeps func_80033CC4's statement-by-statement
     spelling so the base and slot pseudos land in the same registers.
*/

s32 func_800330BC(CardList *list)
{
    s32 row;
    s32 sel;
    s32 icon;
    s32 slot;
    u8 *entry;

    row = list->first | list->cursor;
    if (row != 0) {
        row = (list->first + list->cursor + 1) * 152 /
              list->sort_row_count;
    }
    *(s16 *)(list->scroll_box + 0x32) = row + 0x29;

top:
    if (list->first != list->first_target) {
        if (list->first_target < list->first) {
            *(u16 *)&list->first = *(u16 *)&list->first - 1;
        } else {
            *(u16 *)&list->first = *(u16 *)&list->first + 1;
        }
        func_80031E04(list, 8);
        return 1;
    }

    if ((gInput_wPad1Held & PAD_BUTTON_L1_R1_MASK) != 0) {
        sel = -1;
        row = list->first;
        if ((gInput_wPad1Held & PAD_BUTTON_R1) != 0) {
            if (row == list->row_count - 8 &&
                list->cursor != 7) {
                sel = 7;
            }
            row += 8;
            if (list->row_count - 8 < row) {
                row = list->row_count - 8;
            }
        } else {
            if (row == 0 && list->cursor != 0) {
                sel = 0;
            }
            row -= 8;
        }
        if (row < 0) {
            row = 0;
        }
        list->first_target = row;
        if (list->first != row) {
            SD_SEPlayFull(6);
            goto top;
        }
    finish:
        if (sel >= 0) {
            list->cursor = sel;
            *(s16 *)(list->cursor_box + 0x32) = sel * 22 + 0x2A;
            SD_SEPlayFull(6);
        }
        return 1;
    }

    if ((gInput_wPad1Repeat & PAD_BUTTON_TRIGGER_MASK) != 0) {
        sel = -1;
        row = list->first;
        if ((gInput_wPad1Repeat & PAD_BUTTON_R2) != 0) {
            if (row == list->row_count - 8 &&
                list->cursor != 7) {
                sel = 7;
            }
            row += 0x32;
            if (list->row_count - 8 < row) {
                row = list->row_count - 8;
            }
        } else {
            if (row == 0 && list->cursor != 0) {
                sel = 0;
            }
            row -= 0x32;
            if (row < 0) {
                row = 0;
            }
        }
        if (list->first != row) {
            SD_SEPlayFull(6);
            list->first_target = row;
            list->first = row;
            func_80031E04(list, 8);
            return 1;
        }
        goto finish;
    }

    if ((gInput_wPad1Repeat & PAD_DIRECTION_VERTICAL_MASK) != 0) {
        row = list->first;
        if ((gInput_wPad1Repeat & PAD_DIRECTION_DOWN) != 0) {
            list->cursor += 1;
            if (list->cursor >= 8) {
                row += 1;
                list->cursor = 7;
            }
        } else {
            list->cursor -= 1;
            if (list->cursor < 0) {
                row -= 1;
                list->cursor = 0;
            }
        }
        *(s16 *)(list->cursor_box + 0x32) =
            list->cursor * 22 + 0x2A;
        if (list->row_count - 8 < row) {
            return 1;
        }
        if (row < 0) {
            return 1;
        }
        SD_SEPlayFull(6);
        list->first_target = row;
        if (list->first != row) {
            goto top;
        }
        return 1;
    }

    if ((gInput_wPad1Repeat & (PAD_BUTTON_SELECT | PAD_BUTTON_START)) !=
        0) {
        if ((gInput_wPad1Repeat & PAD_BUTTON_START) != 0) {
            list->sort_choice += 1;
            if (list->sort_choice >= 7) {
                list->sort_choice = 0;
            }
        } else {
            list->sort_choice -= 1;
            if (list->sort_choice < 0) {
                list->sort_choice = 6;
            }
        }
        SD_SEPlayFull(0x2F);
        icon = list->kind;
        slot = list->sort_choice;
        icon = icon << 4;
        slot = slot << 1;
        entry = D_80090DD8 + slot;
        icon = icon + (s32)entry;
        list->sort_mode = *(u8 *)(icon + 1) & 0xF;
        func_80032C48(list);
        return 1;
    }

    return 0;
}
