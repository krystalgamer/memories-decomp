#include "../types.h"
#include "ai.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"
extern signed char gDuel_bOpponentID;
extern unsigned char gDuel_bTerrain;
void AiScript_LoadOpponentID(void)
{
    int index = AiScript_ReadByte();
    register int *values asm("$3") = gAiScript_aMemory;
    register int value asm("$4");

    asm("" : "+r"(values));
    value = gDuel_bOpponentID;
    values[index] = value;
}

void AiScript_LoadTerrain(void)
{
    int index = AiScript_ReadByte();
    register int *values asm("$3") = gAiScript_aMemory;
    register unsigned int value asm("$4");

    asm("" : "+r"(values));
    value = gDuel_bTerrain;
    values[index] = value;
}
