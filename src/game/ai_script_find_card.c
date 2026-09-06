#include "../types.h"
#include "duel_card_layout.h"

struct ActiveCardEntry {
    s16 card_id;
    u8 pad_02[4];
    u16 flags;
    u8 pad_08[4];
};

extern s32 gAiScript_aMemory[];
extern struct ActiveCardEntry gDuel_aActiveCards[];
extern s32 AiScript_ReadByte(void);
extern void Ai_GetCardRange(s32 type, s32 *start, s32 *end);

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
