/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * pinning 2 variables to hard registers and 1 inline asm statement, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/ai_script_load_best_values.c.
 */
#include "../types.h"
#include "../game/ai.h"
#include "../game/ai_script_read_byte.h"
#include "../game/ai_script_commands.h"
/* This unit compiles at -G0, where nothing is placed in small data and a
   plain scalar already gets lui %hi + %lo, so no lever is needed here.
   ai_script_find_best_attack.c spells this same two-byte symbol an array
   because it compiles at -G8 and needs the brackets to escape %gp_rel. The
   two spellings are both load bearing; do not unify them. */
extern unsigned char gAi_bBestAttacker;

void AiScript_LoadBestAttacker(void)
{
    int index = AiScript_ReadByte();
    register int *values asm("$4") = gAiScript_aMemory;
    register unsigned int value asm("$3");

    asm("" : "+r"(values));
    value = gAi_bBestAttacker;
    values[index] = value;
}

