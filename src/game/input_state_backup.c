#define GINPUT_PAD1_HELD_IS_VOLATILE
#define GINPUT_PAD1_REPEAT_IS_VOLATILE
#define GINPUT_PAD2_HELD_IS_VOLATILE
#define GINPUT_PAD2_PRESSED_IS_VOLATILE
#define GINPUT_PAD2_REPEAT_IS_VOLATILE
#define GINPUT_PAD1_PRESSED_IS_VOLATILE
#include "../types.h"
#include "input.h"

extern volatile unsigned short gInput_wPad1RepeatBackup;
extern volatile unsigned short gInput_wPad1PressedBackup;
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
