#include "../types.h"
#include "duel_card_layout.h"

struct ActiveCardEntry {
    s16 card_id;
    s16 attack;
    s16 defense;
    u16 flags;
    u8 pad_08[4];
};

extern s32 gAiScript_aMemory[];
extern struct ActiveCardEntry gDuel_aActiveCards[];
extern s32 AiScript_ReadByte(void);
extern void Ai_GetWinningCardRange(s32 type, s32 *start, s32 *end);
extern s32 Ai_IsCardInSets(s32 sets, s32 slot);

/* AI script opcode taking five operand bytes: a register selecting which
 * statistic to rank by (0 attack, 1 defence, anything else the better of the
 * two), a register holding the zone type, a register that when 1 hides
 * face-down cards in types 3..5, a register naming a set of slots to exclude,
 * and the register to write. It scans the zone's winning-card range and writes
 * the slot index of the strongest card, or 0 when nothing qualifies. Types 1,
 * 4 and 5 pass over a card already used this turn; type 4 wants attack
 * position only and type 5 defence position only. */
void AiScript_FindStrongest(void)
{
    s32 mode;
    s32 type;
    s32 hide_face_down;
    s32 sets;
    s32 result;
    s32 best_slot;
    s32 best_power;
    s32 *table = gAiScript_aMemory;
    s32 start;
    s32 end;
    s32 i;
    s32 v;

    mode = table[AiScript_ReadByte()];
    best_slot = 0;
    type = table[AiScript_ReadByte()];
    best_power = -1;
    hide_face_down = table[AiScript_ReadByte()];
    sets = table[AiScript_ReadByte()];
    result = AiScript_ReadByte();

    Ai_GetWinningCardRange(type, &start, &end);

    for (i = start; i <= end; i++) {
        if (gDuel_aActiveCards[i].card_id == 0) {
            continue;
        }
        if (type == 1 || type == 4 || type == 5) {
            if (gDuel_aActiveCards[i].flags & DUEL_CARD_FLAG_USED_THIS_TURN) {
                continue;
            }
        }
        if (type == 4) {
            if (gDuel_aActiveCards[i].flags &
                DUEL_CARD_FLAG_DEFENSE_POSITION) {
                continue;
            }
        }
        if (type == 5) {
            if (!(gDuel_aActiveCards[i].flags &
                  DUEL_CARD_FLAG_DEFENSE_POSITION)) {
                continue;
            }
        }
        if (type >= 3 && type <= 5) {
            if (gDuel_aActiveCards[i].flags & DUEL_CARD_FLAG_FACE_DOWN) {
                if (hide_face_down == 1) {
                    continue;
                }
            }
        }
        if (Ai_IsCardInSets(sets, i) != 0) {
            continue;
        }
        if (mode != 1) {
            v = gDuel_aActiveCards[i].attack;
            if (best_power < v) {
                best_power = v;
                best_slot = i;
            }
        }
        if (mode == 0) {
            continue;
        }
        v = gDuel_aActiveCards[i].defense;
        if (best_power < v) {
            best_power = v;
            best_slot = i;
        }
    }

    gAiScript_aMemory[result] = best_slot;
}
