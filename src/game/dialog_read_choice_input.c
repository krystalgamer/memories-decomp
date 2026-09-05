#include "../types.h"

extern volatile u16 gInput_wPad1Repeat;
extern u8 gDialog_bChoice;
extern s8 gDialog_bChoiceCount;
extern void SD_SEPlayFull(s32);
extern void Dialog_HighlightChoice(u8 *);

/* Up, down or left on the repeat pad moves the dialog's choice cursor. Down
   wraps to the top; the other two clamp and report the press as handled
   without re-playing the cursor sound.

   gInput_wPad1Repeat is volatile: the target reads it once per test rather
   than caching it, and the three reads are what the three branches turn on. */
s32 Dialog_ReadChoiceInput(u8 *record)
{
    if (gInput_wPad1Repeat & 0x5008) {
        if (gInput_wPad1Repeat & 0x8) {
            u8 choice = gDialog_bChoice;
            s32 count = gDialog_bChoiceCount;

            gDialog_bChoice = choice + 1;
            if ((s8)(choice + 1) >= count) {
                gDialog_bChoice = 0;
            }
        } else if (gInput_wPad1Repeat & 0x4000) {
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
