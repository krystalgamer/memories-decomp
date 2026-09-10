#define GINPUT_PAD1_PRESSED_IS_AGGREGATE
#include "../types.h"
#include "input.h"
#include "duel_check_quit_input.h"

extern signed char gDuel_bOpponentID[];

int Duel_CheckQuitInput(void)
{
    if (gDuel_bOpponentID[0] < 0) {
        if (gInput_wPad1Pressed[0] & PAD_BUTTON_SELECT) {
            gDuel_bQuitDialogState = 1;
            return 1;
        }
    }
    return 0;
}
