#include "../../types.h"
#include "../input.h"
#include "../input_is_pad1_confirm_pressed.h"

unsigned int Input_IsPad1ConfirmPressed(void)
{
    return gInput_awPadPressed[0] &
           (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE);
}
