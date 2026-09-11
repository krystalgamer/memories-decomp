/* Reclassified from matching_c (#3859). This was
 * src/game/dialog_read_choice_input.c, byte-exact only under
 * gcc_2_8_1_cc_g8_as_g1_split, whose compiler and assembler disagree about
 * small data (GCC -G8, MASPSX -G1). Under gcc_2_8_1_g0_split, a single
 * threshold, it is 63 instructions against the target's 65, opcode distance
 * 8. The source below is the match, unchanged apart from its include paths. */
#define GINPUT_PAD1_REPEAT_IS_VOLATILE
#include "../types.h"
#include "../game/dialog_highlight_choice.h"
#include "../game/input.h"
#include "../game/sound.h"
#include "../game/dialog_choice.h"
#include "../unmatched.h"

/* Up, down or R1 on the repeat pad moves the dialog's choice cursor. R1
   wraps to the top; the directions clamp and report the press as handled
   without re-playing the cursor sound.

   gInput_wPad1Repeat is volatile: the target reads it once per test rather
   than caching it, and the three reads are what the three branches turn on. */
s32 Dialog_ReadChoiceInput(u8 *record)
{
    if (gInput_wPad1Repeat &
        (PAD_DIRECTION_VERTICAL_MASK | PAD_BUTTON_R1)) {
        if (gInput_wPad1Repeat & PAD_BUTTON_R1) {
            u8 choice = gDialog_bChoice;
            s32 count = gDialog_bChoiceCount;

            gDialog_bChoice = choice + 1;
            if ((s8)(choice + 1) >= count) {
                gDialog_bChoice = 0;
            }
        } else if (gInput_wPad1Repeat & PAD_DIRECTION_DOWN) {
            u8 choice = gDialog_bChoice;
            s32 count = gDialog_bChoiceCount;

            gDialog_bChoice = choice + 1;
            if ((s8)(choice + 1) >= count) {
                gDialog_bChoice = choice;
                return 1;
            }
        } else {
            u8 choice = gDialog_bChoice;

            gDialog_bChoice = choice - 1;
            if ((s8)(choice - 1) < 0) {
                gDialog_bChoice = choice;
                return 1;
            }
        }
        SD_SEPlayFull(6);
        Dialog_HighlightChoice(record);
        return 1;
    }
    return 0;
}
