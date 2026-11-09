#include "../types.h"
#include "ai.h"
#include "duel_card_layout.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"

/* Each slot is addressed as &gDuel_aActiveCards[index] from the loop index, so the
 * strength reducer keeps one cursor for the card_id, card_type and flags reads,
 * as in retail. */

void AiScript_MatchType(void)
{
    s32 start;
    s32 end;
    AiActiveCard *entry;
    s32 type;
    s32 range;
    s32 set;
    s32 sets;
    s32 output;
    s32 index;

    type = gAiScript_aMemory[AiScript_ReadByte()];
    range = gAiScript_aMemory[AiScript_ReadByte()];
    set = gAiScript_aMemory[AiScript_ReadByte()];
    sets = gAiScript_aMemory[AiScript_ReadByte()];
    output = AiScript_ReadByte();

    Ai_GetCardRange(range, &start, &end);

    for (index = start; index <= end; index++) {
        entry = &gDuel_aActiveCards[index];
        if (entry->card_id == 0) {
            continue;
        }
        if (entry->card_type != type) {
            continue;
        }
        if (range == 1 || range == 3 || range == 6 || range == 8) {
            if ((entry->flags & DUEL_CARD_FLAG_USED_THIS_TURN) != 0) {
                continue;
            }
        }
        if (set == 1 && range >= 5) {
            if ((entry->flags & DUEL_CARD_FLAG_FACE_DOWN) != 0) {
                continue;
            }
        }
        if (Ai_IsCardInSets(sets, index) == 0) {
            break;
        }
    }

    if (end < index) {
        gAiScript_aMemory[output] = 0;
    } else {
        gAiScript_aMemory[output] = index;
    }
}
