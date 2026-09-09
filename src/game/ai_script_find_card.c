#include "../types.h"
#include "duel_card_layout.h"
#include "duel_grid.h"
#include "ai_script_read_byte.h"
#include "ai.h"
#include "ai_script_commands.h"

/* Four contiguous AI script opcode handlers, in address order:
   AiScript_FindDefenseStopper (0x80071CB0), AiScript_CountCards (0x80071EB8),
   AiScript_FindFirstCard (0x80071FC8) and AiScript_FindCard (0x8007214C).
   They are the whole gcc_2_8_1_cc_g0_as_g8_split run above func_80071B64,
   which compiles at gcc_2_8_1_g8_split_no_strength_reduce. All four read
   their operands with AiScript_ReadByte, scan gDuel_aActiveCards, and write
   their answer back into gAiScript_aMemory. */

/* AI script opcode taking two operand bytes: a register that when non-zero
 * makes a face-down opponent card invisible to the scan, and the register to
 * write. It pairs the two fields off strongest against strongest - each round
 * takes the strongest card not yet taken from slots 1..5 and the strongest not
 * yet taken from slots 56..60, marks both taken, and stops as soon as the
 * opponent's pick is not beaten. It answers 1 only if at least one round ran
 * and every round was won, so a losing or empty field answers 0. Slot 0 of
 * gDuel_aActiveCards is the zero entry both searches start from, which is what
 * makes index 0 mean "nothing found" while still comparing cleanly. */
void AiScript_FindDefenseStopper(void)
{
    s32 hide_face_down;
    s32 result;
    s32 answer;
    s32 taken[DUEL_SIDE_COUNT][DUEL_FIELD_ROW_SIZE];
    s32 best;
    s32 other;
    s32 i;
    s32 j;
    AiActiveCard *cards;
    AiActiveCard *others;

    hide_face_down = gAiScript_aMemory[AiScript_ReadByte()];
    result = AiScript_ReadByte();
    answer = 1;

    for (i = 0; i < DUEL_SIDE_COUNT; i++) {
        for (j = 0; j < DUEL_FIELD_ROW_SIZE; j++) {
            taken[i][j] = 0;
        }
    }

    i = 0;
    do {
        best = AI_SLOT_NONE;
        cards = &gDuel_aActiveCards[AI_SLOT_OWN_MONSTER_FIRST];
        for (j = 0; j < DUEL_FIELD_ROW_SIZE; j++) {
            if (taken[0][j] == 0) {
                if (cards[j].attack > gDuel_aActiveCards[best].attack) {
                    best = j + AI_SLOT_OWN_MONSTER_FIRST;
                }
            }
        }
        if (best != AI_SLOT_NONE) {
            taken[0][best - AI_SLOT_OWN_MONSTER_FIRST] = 1;
        }

        other = AI_SLOT_NONE;
        others = &gDuel_aActiveCards[AI_SLOT_OPPONENT_MONSTER_FIRST];
        for (j = 0; j < DUEL_FIELD_ROW_SIZE; j++) {
            if (taken[1][j] == 0) {
                if (hide_face_down == 0 ||
                    !(others[j].flags & DUEL_CARD_FLAG_FACE_DOWN)) {
                    if (others[j].attack > gDuel_aActiveCards[other].attack) {
                        other = j + AI_SLOT_OPPONENT_MONSTER_FIRST;
                    }
                }
            }
        }
        if (other == AI_SLOT_NONE) {
            break;
        }
        taken[1][other - AI_SLOT_OPPONENT_MONSTER_FIRST] = 1;
        if (gDuel_aActiveCards[best].attack <= gDuel_aActiveCards[other].attack) {
            answer = 0;
            break;
        }
        i++;
    } while (i < DUEL_FIELD_ROW_SIZE);

    if (i == 0) {
        answer = 0;
    }
    gAiScript_aMemory[result] = answer;
}

extern void Ai_GetCardRange(s32 type, s32 *start, s32 *end);

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
