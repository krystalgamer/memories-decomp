#include "../types.h"
#include "ai.h"
#include "ai_script_read_byte.h"

/* Same 4-byte-stride table as idx_table_copy_lowbyte.c. */

/* Copies one table entry to another: two indices read from the stream. */
void AiScript_SetRegister(void) {
    s32 src = AiScript_ReadByte();
    s32 dst = AiScript_ReadByte();
    gAiScript_aMemory[dst] = gAiScript_aMemory[src];
}
