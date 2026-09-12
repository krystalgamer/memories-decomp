#include "../types.h"
#include "input.h"

u16 gInput_wPad1Held;
u16 gInput_wPad1Pressed;
u16 gInput_wPad1Repeat;
u16 gInput_wPad1HeldBackup;
u16 gInput_wPad1PressedBackup;
u16 gInput_wPad1RepeatBackup;

void Input_BackupPad1AndUsePad2(void)
{
    u16 value;

    value = *(volatile u16 *)&gInput_wPad1Held;
    gInput_wPad1HeldBackup = value;
    value = *(volatile u16 *)&gInput_wPad1Pressed;
    gInput_wPad1PressedBackup = value;
    value = *(volatile u16 *)&gInput_wPad1Repeat;
    gInput_wPad1RepeatBackup = value;
    value = gInput_wPad2Held;
    gInput_wPad1Held = value;
    value = gInput_wPad2Pressed;
    gInput_wPad1Pressed = value;
    value = gInput_wPad2Repeat;
    gInput_wPad1Repeat = value;
}

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
