#include "../types.h"
#include "input.h"

extern unsigned short gInput_wPad1Pressed[];

unsigned int Input_IsPad1ConfirmPressed(void)
{
    return gInput_wPad1Pressed[0] & PAD_BUTTON_CONFIRM_MASK;
}
