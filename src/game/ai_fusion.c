#include "../types.h"
#include "duel_get_base_card_stat.h"
#include "ai.h"
#include "ai_constants.h"
#include "card_constants.h"
#include "duel_check_ritual.h"
#include "duel_card.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"
#include "duel_card_checks.h"
#include "duel_grid.h"

extern u8 D_800EAE88[];

void AiScript_FindEquipTarget(void)
{
    s32 owner = gAiScript_aMemory[AiScript_ReadByte()];
    s32 mode = gAiScript_aMemory[AiScript_ReadByte()];
    s32 type = gAiScript_aMemory[AiScript_ReadByte()];
    s32 dest = AiScript_ReadByte();
    s32 i;

    for (i = 1; i < DUEL_FIELD_ROW_SIZE + 1; i++) {
        AiActiveCard *card = &gDuel_aActiveCards[i];

        if (!card->card_id)
            continue;
        if (mode == 1 && (card->flags & DUEL_CARD_FLAG_USED_THIS_TURN))
            continue;
        if (Ai_IsCardInSets(type, i))
            continue;
        if (Duel_CheckEquip(owner, card->card_id)) {
            gAiScript_aMemory[dest] = i;
            return;
        }
    }
    gAiScript_aMemory[dest] = 0;
}

void AiScript_CheckRitual(void)
{
    s32 *memory = gAiScript_aMemory;
    s32 value = memory[AiScript_ReadByte()];
    s32 dest = AiScript_ReadByte();

    if (Duel_CheckRitual(0, value) != 0)
        memory[dest] = 1;
    else
        memory[dest] = 0;
}

void AiScript_FindFirstMonster(void)
{
    s32 dest = AiScript_ReadByte();
    s32 i;

    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
        s32 index = D_800EAE88[i];
        AiActiveCard *card = &gDuel_aActiveCards[index];

        if (card->card_id && card->card_type < CARD_TYPE_MAGIC) {
            gAiScript_aMemory[dest] = index;
            return;
        }
    }
    gAiScript_aMemory[dest] = 0;
}

void AiScript_FindFirstType(void)
{
    s32 *memory = gAiScript_aMemory;
    s32 type = memory[AiScript_ReadByte()];
    s32 dest = AiScript_ReadByte();
    s32 i = 0;
    u8 *indices = D_800EAE88;
    AiActiveCard *cards = gDuel_aActiveCards;

    do {
        s32 index = *(u8 *)(i + (s32)indices);
        AiActiveCard *card =
            (AiActiveCard *)(index * sizeof(AiActiveCard) + (s32)cards);

        if (card->card_id != 0 && card->card_type == type) {
            memory[dest] = index;
            return;
        }
        i++;
    } while (i < DUEL_FIELD_ROW_SIZE);

    gAiScript_aMemory[dest] = 0;
}

void Ai_CompleteFusion(s32 arg0)
{
    s32 i;
    s32 index;
    s32 card_id;
    s32 result;
    s32 j;

    i = 0;
    if (gAiScript_State.fusion_count == 0)
        return;
    do {
        if (gAiScript_State.fusion_used[i] == 0) {
            card_id = gDuel_aActiveCards[i + AI_SLOT_OWN_HAND_FIRST].card_id;
            index = i + AI_SLOT_OWN_HAND_FIRST;
            if (card_id != 0) {
                if (Ai_IsCardInSets(gAiScript_State.fusion_set, index) == 0) {
                    result = Duel_CheckFusion(arg0, card_id);
                    if (result == 0)
                        result = Duel_CheckEquip(arg0, card_id);
                    if (result != 0) {
                        gAiScript_State.fusion_path[gAiScript_State.fusion_depth] = index;
                        if (
                            Duel_GetBaseCardStat(result, 0) >
                                gAiScript_State.fusion_best_stat ||
                            (
                                Duel_GetBaseCardStat(result, 0) ==
                                    gAiScript_State.fusion_best_stat &&
                                gAiScript_State.fusion_depth <
                                    gAiScript_State.fusion_best_depth
                            )
                        ) {
                            gAiScript_State.fusion_best_stat =
                                Duel_GetBaseCardStat(result, 0);
                            gAiScript_State.fusion_best_depth =
                                gAiScript_State.fusion_depth;
                            for (j = 0;
                                 j <= (s32)gAiScript_State.fusion_best_depth;
                                 j++)
                                gAiScript_State.combo_cards[j] =
                                    gAiScript_State.fusion_path[j];
                            gAiScript_State.combo_cards[j] = 0;
                        }
                        if (
                            Duel_GetBaseCardStat(result, 1) >
                                gAiScript_State.fusion_best_stat ||
                            (
                                Duel_GetBaseCardStat(result, 1) ==
                                    gAiScript_State.fusion_best_stat &&
                                gAiScript_State.fusion_depth <
                                    gAiScript_State.fusion_best_depth
                            )
                        ) {
                            gAiScript_State.fusion_best_stat =
                                Duel_GetBaseCardStat(result, 1);
                            gAiScript_State.fusion_best_depth =
                                gAiScript_State.fusion_depth;
                            for (j = 0;
                                 j <= (s32)gAiScript_State.fusion_best_depth;
                                 j++)
                                gAiScript_State.combo_cards[j] =
                                    gAiScript_State.fusion_path[j];
                            gAiScript_State.combo_cards[j] = 0;
                        }
                        if (gAiScript_State.fusion_depth <
                            gAiScript_State.fusion_limit - 1) {
                            gAiScript_State.fusion_used[i] = 1;
                            gAiScript_State.fusion_depth++;
                            Ai_CompleteFusion(result);
                            gAiScript_State.fusion_used[i] = 0;
                            gAiScript_State.fusion_depth--;
                        }
                    }
                }
            }
        }
        i++;
    } while (i < gAiScript_State.fusion_count);
}

/* The two fusion-search opcodes below seed the fusion scratch in
   gAiScript_State and recurse through Ai_CompleteFusion above. They were
   recorded at gcc_2_8_1_cc_g0_as_g8_split and gcc_2_8_1_g8_split, and each
   compiles to an identical object at this unit's gcc_2_8_1_g0_split. */

/* AI script opcode taking three operand bytes: a register holding the search
 * depth minus one, a register naming a set of slots to exclude, and the
 * register to write. It seeds the fusion scratch in gAiScript_State - hand size
 * at 0x9C, depth limit at 0x9D, excluded sets at 0x9E, best power at 0xA0,
 * current depth at 0xA2, the current path from 0xA4 and per-slot in-use flags
 * from 0xAA - then walks the five field slots and then the hand, recording the
 * strongest card by either statistic and recursing through Ai_CompleteFusion
 * once the depth allows it. The selected combo is stored separately at 0x38,
 * with its inclusive best-depth bound at 0xA3.
 * The answer classifies where the best combo starts:
 * 3 when nothing was found, 0 when it starts past the field, otherwise 1 or 2
 * on the second combo byte. */
void AiScript_FindBestCombo(void)
{
    s32 depth;
    s32 sets;
    s32 dest;
    s32 n;
    s32 i;
    s32 slot;
    s32 card;
    u8 *e;
    s32 *table;

    table = gAiScript_aMemory;
    depth = table[AiScript_ReadByte()];
    sets = table[AiScript_ReadByte()];
    depth = depth + 1;
    dest = AiScript_ReadByte();
    n = Ai_GetHandSize();

    gAiScript_State.fusion_count = n;
    gAiScript_State.fusion_best_stat = 0;
    gAiScript_State.fusion_depth = 0;
    gAiScript_State.fusion_limit = depth;
    gAiScript_State.fusion_set = sets;

    for (i = 0; i < gAiScript_State.fusion_count; i++) {
        gAiScript_State.fusion_used[i] = 0;
    }

    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
        card = gDuel_aActiveCards[i + AI_SLOT_OWN_MONSTER_FIRST].card_id;
        slot = i + 1;
        if (card == 0) {
            continue;
        }
        if (Ai_IsCardInSets(sets, slot) != 0) {
            continue;
        }
        gAiScript_State.fusion_path[gAiScript_State.fusion_depth] = slot;
        if (gDuel_aActiveCards[i + AI_SLOT_OWN_MONSTER_FIRST].attack >
            gAiScript_State.fusion_best_stat) {
            gAiScript_State.fusion_best_stat =
                gDuel_aActiveCards[i + AI_SLOT_OWN_MONSTER_FIRST].attack;
            gAiScript_State.combo_cards[0] = slot;
            gAiScript_State.combo_cards[1] = 0;
            gAiScript_State.fusion_best_depth =
                gAiScript_State.fusion_depth;
        }
        if (gDuel_aActiveCards[i + AI_SLOT_OWN_MONSTER_FIRST].defense >
            gAiScript_State.fusion_best_stat) {
            gAiScript_State.fusion_best_stat =
                gDuel_aActiveCards[i + AI_SLOT_OWN_MONSTER_FIRST].defense;
            gAiScript_State.combo_cards[0] = slot;
            gAiScript_State.combo_cards[1] = 0;
            gAiScript_State.fusion_best_depth =
                gAiScript_State.fusion_depth;
        }
        if (gAiScript_State.fusion_limit >= 2) {
            /* e deliberately points at the state base plus i, not at
             * &fusion_used[i]: keeping fusion_used's 0xAA in the access
             * displacement lets the address arithmetic fill the branch delay
             * slot. Spelling it &fusion_used[i] costs two instructions. */
            e = (u8 *)&gAiScript_State + i;
            e[AI_SCRIPT_FUSION_USED_BYTE_OFFSET] = 1;
            gAiScript_State.fusion_depth =
                gAiScript_State.fusion_depth + 1;
            Ai_CompleteFusion(card);
            e[AI_SCRIPT_FUSION_USED_BYTE_OFFSET] = 0;
            gAiScript_State.fusion_depth =
                gAiScript_State.fusion_depth - 1;
        }
    }

    for (i = 0; i < gAiScript_State.fusion_count; i++) {
        card = gDuel_aActiveCards[i + AI_SLOT_OWN_HAND_FIRST].card_id;
        slot = i + AI_SLOT_OWN_HAND_FIRST;
        if (card == 0) {
            continue;
        }
        if (Ai_IsCardInSets(sets, slot) != 0) {
            continue;
        }
        gAiScript_State.fusion_path[gAiScript_State.fusion_depth] = slot;
        if (gDuel_aActiveCards[i + AI_SLOT_OWN_HAND_FIRST].attack >
            gAiScript_State.fusion_best_stat) {
            gAiScript_State.fusion_best_stat =
                gDuel_aActiveCards[i + AI_SLOT_OWN_HAND_FIRST].attack;
            gAiScript_State.combo_cards[0] = slot;
            gAiScript_State.combo_cards[1] = 0;
            gAiScript_State.fusion_best_depth =
                gAiScript_State.fusion_depth;
        }
        if (gDuel_aActiveCards[i + AI_SLOT_OWN_HAND_FIRST].defense >
            gAiScript_State.fusion_best_stat) {
            gAiScript_State.fusion_best_stat =
                gDuel_aActiveCards[i + AI_SLOT_OWN_HAND_FIRST].defense;
            gAiScript_State.combo_cards[0] = slot;
            gAiScript_State.combo_cards[1] = 0;
            gAiScript_State.fusion_best_depth =
                gAiScript_State.fusion_depth;
        }
        if (gAiScript_State.fusion_limit >= 3) {
            /* e deliberately points at the state base plus i, not at
             * &fusion_used[i]: keeping fusion_used's 0xAA in the access
             * displacement lets the address arithmetic fill the branch delay
             * slot. Spelling it &fusion_used[i] costs two instructions. */
            e = (u8 *)&gAiScript_State + i;
            e[AI_SCRIPT_FUSION_USED_BYTE_OFFSET] = 1;
            gAiScript_State.fusion_depth =
                gAiScript_State.fusion_depth + 1;
            Ai_CompleteFusion(card);
            e[AI_SCRIPT_FUSION_USED_BYTE_OFFSET] = 0;
            gAiScript_State.fusion_depth =
                gAiScript_State.fusion_depth - 1;
        }
    }

    if (gAiScript_State.combo_cards[0] != 0) {
        if (gAiScript_State.combo_cards[0] >= DUEL_FIELD_ROW_SIZE + 1) {
            gAiScript_aMemory[dest] = 0;
        } else if (gAiScript_State.combo_cards[1] != 0) {
            gAiScript_aMemory[dest] = 1;
        } else {
            gAiScript_aMemory[dest] = 2;
        }
    } else {
        gAiScript_aMemory[dest] = 3;
    }
}

void AiScript_EvaluateFusion(void)
{
    s32 a;
    s32 b;
    s32 c;
    s32 k;
    s32 x;
    s32 y;
    s32 i;
    s32 n;

    a = gAiScript_aMemory[AiScript_ReadByte()];
    b = gAiScript_aMemory[AiScript_ReadByte()] + 1;
    c = gAiScript_aMemory[AiScript_ReadByte()];
    k = AiScript_ReadByte();
    n = Ai_GetHandSize();

    gAiScript_State.fusion_count = n;
    gAiScript_State.fusion_limit = b;
    gAiScript_State.fusion_best_stat = 0;
    gAiScript_State.fusion_depth = 0;
    gAiScript_State.fusion_best_depth = 0;
    gAiScript_State.fusion_set = c;

    for (i = 0; i < gAiScript_State.fusion_count; i++) {
        gAiScript_State.fusion_used[i] = 0;
    }

    x = Duel_GetBaseCardStat(a, 0);
    y = Duel_GetBaseCardStat(a, 1);

    if (y < x) {
        gAiScript_State.fusion_best_stat = Duel_GetBaseCardStat(a, 0);
    } else {
        gAiScript_State.fusion_best_stat = Duel_GetBaseCardStat(a, 1);
    }

    Ai_CompleteFusion(a);

    if (gAiScript_State.fusion_best_depth != 0) {
        gAiScript_aMemory[k] = 0;
    } else {
        gAiScript_aMemory[k] = 1;
    }
}
