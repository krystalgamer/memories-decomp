#define GINPUT_PAD1_BACKUPS_ARE_VOLATILE
#include "../types.h"
#include "input.h"

u16 gInput_wPad1Held;
u16 gInput_wPad1Pressed;
u16 gInput_wPad1Repeat;

void Input_RestorePad1FromBackup(void)
{
    u16 value;

    value = gInput_wPad1HeldBackup;
    gInput_wPad1Held = value;
    value = gInput_wPad1PressedBackup;
    gInput_wPad1Pressed = value;
    value = gInput_wPad1RepeatBackup;
    gInput_wPad1Repeat = value;
}
