#define GINPUT_PAD1_PRESSED_IS_AGGREGATE
#include "../types.h"
#include "input.h"

unsigned int Input_IsPad1ConfirmPressed(void)
{
    return gInput_wPad1Pressed[0] & PAD_BUTTON_CONFIRM_MASK;
}
