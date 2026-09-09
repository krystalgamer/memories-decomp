#include "../types.h"
#include "ai.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"
extern u8 gAiScript_State[];
void AiScript_ClearCards(void)
{
    s32 i = AI_SCRIPT_CARD_SET_COUNT - 1;
    u16 *entries = (u16 *)gAiScript_State;

    entries += AI_SCRIPT_CARD_SET_HALFWORD_OFFSET;
    do {
        entries[AI_SCRIPT_CARD_SET_COUNT - 1] = AI_SCRIPT_CARD_SET_EMPTY;
        i--;
        entries--;
    } while (i >= 0);
}

void AiScript_AddType(void)
{
    s32 *memory = gAiScript_aMemory;
    s32 index = AiScript_ReadByte();
    s32 type = memory[index] + AI_SCRIPT_TYPE_SET_ENCODING_BIAS;
    s32 i;

    for (i = 0; i < AI_SCRIPT_TYPE_SET_COUNT; i++) {
        u8 *entry = gAiScript_State + i;

        if (entry[AI_SCRIPT_TYPE_SET_BYTE_OFFSET] == type) {
            break;
        }
        if (entry[AI_SCRIPT_TYPE_SET_BYTE_OFFSET] == AI_SCRIPT_TYPE_SET_EMPTY) {
            entry[AI_SCRIPT_TYPE_SET_BYTE_OFFSET] = type;
            break;
        }
    }
}

void AiScript_ClearTypes(void)
{
    s32 i = AI_SCRIPT_TYPE_SET_COUNT - 1;
    u8 *entries = gAiScript_State;

    entries += i;
    do {
        entries[AI_SCRIPT_TYPE_SET_BYTE_OFFSET] = AI_SCRIPT_TYPE_SET_EMPTY;
        i--;
        entries--;
    } while (i >= 0);
}
