/* Reclassified from matching_c (#3859). This was part of
 * src/game/ai_script_find_card.c. It is byte-exact under gcc_2_8_1_g0_split
 * only with 2 variables pinned to hard registers; the mixed-G
 * gcc_2_8_1_cc_g0_as_g8_split it was first matched at turned out to be
 * inert. Without the pins it is 68 of 68 instructions with 9 differing,
 * opcode distance 0. The source below is the match, unchanged apart from
 * its include paths. */
#include "../types.h"
#include "../game/duel_card_layout.h"
#include "../game/duel_grid.h"
#include "../game/ai_script_read_byte.h"
#include "../game/ai.h"
#include "../game/ai_script_commands.h"

void AiScript_CountCards(void)
{
    register s32 type __asm__("s0");
    register s32 count __asm__("s1");
    s32 *table = gAiScript_aMemory;
    s32 idx1;
    s32 idx2;
    s32 start;
    s32 end;
    s32 i;

    idx1 = AiScript_ReadByte();
    type = table[idx1];
    idx2 = AiScript_ReadByte();
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

    gAiScript_aMemory[idx2] = count;
}
