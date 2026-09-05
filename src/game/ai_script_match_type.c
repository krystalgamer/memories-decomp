#include "../types.h"
#include "ai.h"
#include "duel_card_layout.h"

extern s32 AiScript_ReadByte(void);
extern void Ai_GetCardRange(s32, s32 *, s32 *);
extern s32 Ai_IsCardInSets(s32, s32);

void AiScript_MatchType(void)
{
    s32 start;
    s32 end;
    AiActiveCard *entry;
    AiActiveCard *cards;
    s32 type;
    s32 range;
    s32 set;
    s32 sets;
    s32 output;
    s32 index;
    s32 one;

    type = gAiScript_aMemory[AiScript_ReadByte()];
    range = gAiScript_aMemory[AiScript_ReadByte()];
    set = gAiScript_aMemory[AiScript_ReadByte()];
    sets = gAiScript_aMemory[AiScript_ReadByte()];
    output = AiScript_ReadByte();

    Ai_GetCardRange(range, &start, &end);

    index = start;
    if (end < index) {
        goto zero;
    }

    one = 1;
    cards = gDuel_aActiveCards;
    entry = &cards[index];
    while (1) {
        if (entry->card_id == 0) {
            goto next;
        }
        if (entry->card_type != type) {
            goto next;
        }
        if (range == one || range == 3 || range == 6 || range == 8) {
            if ((entry->flags & DUEL_CARD_FLAG_USED_THIS_TURN) != 0) {
                goto next;
            }
        }
        if (set == one && range >= 5) {
            if ((entry->flags & 0x1000) != 0) {
                goto next;
            }
        }
        if (Ai_IsCardInSets(sets, index) == 0) {
            break;
        }
next:
        entry++;
        index++;
        if (end < index) {
            break;
        }
    }

    if (end < index) {
zero:
        gAiScript_aMemory[output] = 0;
    } else {
        gAiScript_aMemory[output] = index;
    }
}
