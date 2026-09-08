#include "../types.h"
#include "duel_card_layout.h"
#include "duel_grid.h"

struct ActiveCardEntry {
    s16 card_id;
    s16 power;
    u8 pad_04[2];
    u16 flags;
    u8 pad_08[4];
};

extern s32 gAiScript_aMemory[];
extern struct ActiveCardEntry gDuel_aActiveCards[];
extern s32 AiScript_ReadByte(void);

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
    struct ActiveCardEntry *cards;
    struct ActiveCardEntry *others;

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
        best = 0;
        cards = &gDuel_aActiveCards[1];
        for (j = 0; j < DUEL_FIELD_ROW_SIZE; j++) {
            if (taken[0][j] == 0) {
                if (cards[j].power > gDuel_aActiveCards[best].power) {
                    best = j + 1;
                }
            }
        }
        if (best != 0) {
            taken[0][best - 1] = 1;
        }

        other = 0;
        others = &gDuel_aActiveCards[56];
        for (j = 0; j < DUEL_FIELD_ROW_SIZE; j++) {
            if (taken[1][j] == 0) {
                if (hide_face_down == 0 ||
                    !(others[j].flags & DUEL_CARD_FLAG_FACE_DOWN)) {
                    if (others[j].power > gDuel_aActiveCards[other].power) {
                        other = j + 56;
                    }
                }
            }
        }
        if (other == 0) {
            break;
        }
        taken[1][other - 56] = 1;
        if (gDuel_aActiveCards[best].power <= gDuel_aActiveCards[other].power) {
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
