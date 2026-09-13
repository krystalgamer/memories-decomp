#include "../types.h"
#include "duel_card_layout.h"
#include "duel_grid.h"
#include "ai_script_read_byte.h"
#include "ai.h"
#include "ai_script_commands.h"

/* Two contiguous AI script opcode handlers that scan a zone's slot range for
   the first match: state-based AiScript_FindFirstCard (0x80071FC8) and
   card-ID-based AiScript_FindCard (0x8007214C). Both read their operands
   with AiScript_ReadByte, scan gDuel_aActiveCards, and write their answer
   back into gAiScript_aMemory. */

/* AI script opcode taking three operand bytes: a register holding the wanted
 * slot state, a register holding the zone type, and the register to write.
 * It walks the zone's slot range in order and stops at the first slot whose
 * state matches - 0 for an empty slot, 1 for a face-up card, 2 for a face-down
 * card - writing that slot index, or 0 when the range runs out. In the four
 * zone types that can act, a card already used this turn is passed over.
 * Slot 0 doubles as the not-found answer, which is why the caller's range
 * never starts there. */
void AiScript_FindFirstCard(void)
{
    s32 wanted;
    s32 type;
    s32 result;
    s32 *table = gAiScript_aMemory;
    s32 wanted_idx;
    s32 type_idx;
    s32 start;
    s32 end;
    s32 i;

    wanted_idx = AiScript_ReadByte();
    wanted = table[wanted_idx];
    type_idx = AiScript_ReadByte();
    type = table[type_idx];
    result = AiScript_ReadByte();

    Ai_GetCardRange(type, &start, &end);

    for (i = start; i <= end; i++) {
        if (type == 1 || type == 3 || type == 6 || type == 8) {
            if (gDuel_aActiveCards[i].flags & DUEL_CARD_FLAG_USED_THIS_TURN) {
                continue;
            }
        }
        if (gDuel_aActiveCards[i].card_id != 0) {
            if (gDuel_aActiveCards[i].flags & DUEL_CARD_FLAG_FACE_DOWN) {
                if (wanted == 2) {
                    break;
                }
            } else {
                if (wanted == 1) {
                    break;
                }
            }
        } else {
            if (wanted == 0) {
                break;
            }
        }
    }

    if (end < i) {
        gAiScript_aMemory[result] = 0;
    } else {
        gAiScript_aMemory[result] = i;
    }
}

/* AI script opcode taking four operand bytes: a register holding the card id
 * to look for, a register holding the zone type, a register that when 1 makes
 * a face-down card in a hidden zone (type 5 and up) not count, and the
 * register to write. It walks the zone's slot range in order and stops at the
 * first slot holding that card id, writing the slot index, or 0 when the range
 * runs out. In the four zone types that can act, a card already used this turn
 * is passed over. */
void AiScript_FindCard(void)
{
    s32 wanted;
    s32 type;
    s32 visible_only;
    s32 result;
    s32 *table = gAiScript_aMemory;
    s32 wanted_idx;
    s32 type_idx;
    s32 visible_idx;
    s32 start;
    s32 end;
    s32 i;

    wanted_idx = AiScript_ReadByte();
    wanted = table[wanted_idx];
    type_idx = AiScript_ReadByte();
    type = table[type_idx];
    visible_idx = AiScript_ReadByte();
    visible_only = table[visible_idx];
    result = AiScript_ReadByte();

    Ai_GetCardRange(type, &start, &end);

    for (i = start; i <= end; i++) {
        if (gDuel_aActiveCards[i].card_id != wanted) {
            continue;
        }
        if (type == 1 || type == 3 || type == 6 || type == 8) {
            if (gDuel_aActiveCards[i].flags & DUEL_CARD_FLAG_USED_THIS_TURN) {
                continue;
            }
        }
        if (visible_only != 1) {
            break;
        }
        if (type < 5) {
            break;
        }
        if (!(gDuel_aActiveCards[i].flags & DUEL_CARD_FLAG_FACE_DOWN)) {
            break;
        }
    }

    if (end < i) {
        gAiScript_aMemory[result] = 0;
    } else {
        gAiScript_aMemory[result] = i;
    }
}
