#include "../types.h"

extern volatile unsigned short gInput_wPad1Repeat;
extern volatile unsigned short gInput_wPad2Repeat;
extern volatile unsigned short gInput_wPad1Pressed;
extern volatile unsigned short gInput_wPad2Pressed;
extern volatile unsigned short D_8009B39E;
extern volatile unsigned short D_8009B3A0;
extern volatile unsigned short gInput_wPad1Held;
extern volatile unsigned short gInput_wPad2Held;
extern volatile unsigned short D_8009B3AC;

void func_8003CDF8(void)
{
    unsigned short value;

    value = gInput_wPad1Held;
    __asm__ volatile("nop");
    D_8009B3AC = value;
    value = gInput_wPad1Pressed;
    __asm__ volatile("nop");
    D_8009B3A0 = value;
    value = gInput_wPad1Repeat;
    __asm__ volatile("nop");
    D_8009B39E = value;
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

void func_8003CE48(void)
{
    unsigned short value;

    value = D_8009B3AC;
    __asm__ volatile("nop");
    gInput_wPad1Held = value;
    value = D_8009B3A0;
    __asm__ volatile("nop");
    gInput_wPad1Pressed = value;
    value = D_8009B39E;
    __asm__ volatile("nop");
    gInput_wPad1Repeat = value;
}
