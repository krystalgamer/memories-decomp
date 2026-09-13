#include "../types.h"
#include "duel_card_layout.h"
#include "duel_grid.h"
#include "ai_script_read_byte.h"
#include "ai.h"
#include "ai_script_commands.h"

void AiScript_CountCards(void)
{
    s32 count;
    s32 *table = gAiScript_aMemory;
    s32 type;
    s32 output;
    s32 start;
    s32 end;
    s32 i;

    type = AiScript_ReadByte();
    type = table[type];
    output = AiScript_ReadByte();
    count = 0;

    Ai_GetCardRange(type, &start, &end);

    for (i = start; i <= end; i++) {
        AiActiveCard *entry = &gDuel_aActiveCards[i];
        if (entry->card_id != 0) {
            if (type == 1 || type == 3 || type == 6 || type == 8) {
                if (!(entry->flags & DUEL_CARD_FLAG_USED_THIS_TURN)) {
                    count++;
                }
            } else {
                count++;
            }
        }
    }

    gAiScript_aMemory[output] = count;
}
