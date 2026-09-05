#include "../types.h"

extern volatile unsigned short gInput_wPad1Repeat;
extern volatile unsigned short gInput_wPad2Repeat;
extern volatile unsigned short gInput_wPad1Pressed;
extern volatile unsigned short gInput_wPad2Pressed;
extern volatile unsigned short gInput_wPad1RepeatBackup;
extern volatile unsigned short gInput_wPad1PressedBackup;
extern volatile unsigned short gInput_wPad1Held;
extern volatile unsigned short gInput_wPad2Held;
extern volatile unsigned short gInput_wPad1HeldBackup;

void Input_BackupPad1AndUsePad2(void)
{
    unsigned short value;

    value = gInput_wPad1Held;
    __asm__ volatile("nop");
    gInput_wPad1HeldBackup = value;
    value = gInput_wPad1Pressed;
    __asm__ volatile("nop");
    gInput_wPad1PressedBackup = value;
    value = gInput_wPad1Repeat;
    __asm__ volatile("nop");
    gInput_wPad1RepeatBackup = value;
    value = gInput_wPad2Held;
    __asm__ volatile("nop");
    gInput_wPad1Held = value;
    value = gInput_wPad2Pressed;
    __asm__ volatile("nop");
    gInput_wPad1Pressed = value;
    value = gInput_wPad2Repeat;
    __asm__ volatile("nop");
    gInput_wPad1Repeat = value;
}

void Input_RestorePad1FromBackup(void)
{
    unsigned short value;

    value = gInput_wPad1HeldBackup;
    __asm__ volatile("nop");
    gInput_wPad1Held = value;
    value = gInput_wPad1PressedBackup;
    __asm__ volatile("nop");
    gInput_wPad1Pressed = value;
    value = gInput_wPad1RepeatBackup;
    __asm__ volatile("nop");
    gInput_wPad1Repeat = value;
}
