#include "../types.h"
#include "duel_card_layout.h"
#include "duel_grid.h"
#include "ai_script_read_byte.h"
#include "ai.h"
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
