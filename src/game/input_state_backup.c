#include "../types.h"

extern volatile unsigned short D_8009B394;
extern volatile unsigned short D_8009B396;
extern volatile unsigned short D_8009B398;
extern volatile unsigned short D_8009B39A;
extern volatile unsigned short D_8009B39E;
extern volatile unsigned short D_8009B3A0;
extern volatile unsigned short D_8009B3A4;
extern volatile unsigned short D_8009B3A6;
extern volatile unsigned short D_8009B3AC;

void func_8003CDF8(void)
{
    unsigned short value;

    value = D_8009B3A4;
    __asm__ volatile("nop");
    D_8009B3AC = value;
    value = D_8009B398;
    __asm__ volatile("nop");
    D_8009B3A0 = value;
    value = D_8009B394;
    __asm__ volatile("nop");
    D_8009B39E = value;
    value = D_8009B3A6;
    __asm__ volatile("nop");
    D_8009B3A4 = value;
    value = D_8009B39A;
    __asm__ volatile("nop");
    D_8009B398 = value;
    value = D_8009B396;
    __asm__ volatile("nop");
    D_8009B394 = value;
}

void func_8003CE48(void)
{
    unsigned short value;

    value = D_8009B3AC;
    __asm__ volatile("nop");
    D_8009B3A4 = value;
    value = D_8009B3A0;
    __asm__ volatile("nop");
    D_8009B398 = value;
    value = D_8009B39E;
    __asm__ volatile("nop");
    D_8009B394 = value;
}
