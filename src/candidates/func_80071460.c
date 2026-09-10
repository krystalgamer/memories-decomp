/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * pinning 2 variables to hard registers and 1 inline asm statement, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/ai_script_load_duel_state.c.
 */
#include "../types.h"
#include "../game/ai.h"
#include "../game/ai_script_read_byte.h"
#include "../game/ai_script_commands.h"
extern unsigned char gDuel_bTerrain;

void AiScript_LoadTerrain(void)
{
    int index = AiScript_ReadByte();
    register int *values asm("$3") = gAiScript_aMemory;
    register unsigned int value asm("$4");

    asm("" : "+r"(values));
    value = gDuel_bTerrain;
    values[index] = value;
}
