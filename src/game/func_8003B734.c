#include "../types.h"
#include "input.h"

extern unsigned short D_8009B398[];

unsigned int func_8003B734(void)
{
    return D_8009B398[0] & PAD_BUTTON_CONFIRM_MASK;
}
