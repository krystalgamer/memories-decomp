#include "../types.h"
#include "ai.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"
/* This unit compiles at -G0, where nothing is placed in small data and a
   plain scalar already gets lui %hi + %lo, so no lever is needed here.
   ai_script_find_best_attack.c spells this same two-byte symbol an array
   because it compiles at -G8 and needs the brackets to escape %gp_rel. The
   two spellings are both load bearing; do not unify them. */
extern unsigned short gAi_wBestDifference;
extern unsigned char gAi_bBestAttacker;
extern unsigned char gAi_bBestTarget;
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
