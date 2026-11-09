#include "../types.h"
#include "ai.h"
#include "duel_card_layout.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"

/* Each slot is indexed from gDuel_aActiveCards; gcc's strength reduction
 * turns the index into retail's single cursor, so the unit needs no
 * -fno-strength-reduce. A walking AiActiveCard cursor instead makes it build
 * a second induction variable for the repeated flag reads. */
void AiScript_MatchType(void)
{
    s32 start;
    s32 end;
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
    while (1) {
        if (gDuel_aActiveCards[index].card_id == 0) {
            goto next;
        }
        if (gDuel_aActiveCards[index].card_type != type) {
            goto next;
        }
        if (range == one || range == 3 || range == 6 || range == 8) {
            if ((gDuel_aActiveCards[index].flags & DUEL_CARD_FLAG_USED_THIS_TURN) != 0) {
                goto next;
            }
        }
        if (set == one && range >= 5) {
            if ((gDuel_aActiveCards[index].flags & DUEL_CARD_FLAG_FACE_DOWN) != 0) {
                goto next;
            }
        }
        if (Ai_IsCardInSets(sets, index) == 0) {
            break;
        }
next:
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
