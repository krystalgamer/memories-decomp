#include "../types.h"
#include "input.h"

extern unsigned short gInput_wPad1Pressed[];

unsigned int func_8003B734(void)
{
    return gInput_wPad1Pressed[0] & PAD_BUTTON_CONFIRM_MASK;
}
