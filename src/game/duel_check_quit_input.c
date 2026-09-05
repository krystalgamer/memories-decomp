#include "../types.h"
#include "input.h"

extern signed char gDuel_bOpponentID[];
extern unsigned short gInput_wPad1Pressed[];
extern unsigned char gDuel_bQuitDialogState;

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
