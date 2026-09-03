#include "../types.h"

extern int gAiScript_aMemory[];
extern unsigned short gAi_wBestDifference;
extern unsigned char gAi_bBestAttacker;
extern unsigned char gAi_bBestTarget;
extern int AiScript_ReadByte(void);

void AiScript_LoadBestDifference(void)
{
    int index = AiScript_ReadByte();
    register int *values asm("$4") = gAiScript_aMemory;
    register unsigned int value asm("$3");

    asm("" : "+r"(values));
    value = gAi_wBestDifference;
    values[index] = value;
}

void AiScript_LoadBestAttacker(void)
{
    int index = AiScript_ReadByte();
    register int *values asm("$4") = gAiScript_aMemory;
    register unsigned int value asm("$3");

    asm("" : "+r"(values));
    value = gAi_bBestAttacker;
    values[index] = value;
}

void AiScript_LoadBestTarget(void)
{
    int index = AiScript_ReadByte();
    register int *values asm("$4") = gAiScript_aMemory;
    register unsigned int value asm("$3");

    asm("" : "+r"(values));
    value = gAi_bBestTarget;
    values[index] = value;
}
