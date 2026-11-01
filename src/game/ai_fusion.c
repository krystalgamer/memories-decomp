#include "../types.h"
#include "duel_card_stat_rules.h"
#include "ai.h"
#include "ai_constants.h"
#include "card_constants.h"
#include "duel_check_ritual.h"
#include "duel_card.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"
#include "duel_card_checks.h"
#include "duel_grid.h"
#include "../unmatched.h"
#include "main_run_campaign_data.h"

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
        s32 index = gAi_Selection.bytes[i];
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
    u8 *indices = gAi_Selection.bytes;
    AiActiveCard *cards = gDuel_aActiveCards;

    do {
        s32 index = *(u8 *)(i + (s32)indices);
        AiActiveCard *card =
            (AiActiveCard *)((u32)&((AiActiveCard *)0)[index] + (s32)cards);

        if (card->card_id != 0 && card->card_type == type) {
            memory[dest] = index;
            return;
        }
        i++;
    } while (i < DUEL_FIELD_ROW_SIZE);

    gAiScript_aMemory[dest] = 0;
}

void Ai_CompleteFusion(s32 candidate_card_id)
{
    s32 hand_index;
    s32 hand_slot_index;
    s32 hand_card_id;
    s32 result_card_id;
    s32 path_index;

    hand_index = 0;
    if (gAiScript_State.fusion_count == 0)
        return;
    do {
        if (gAiScript_State.fusion_used[hand_index] == 0) {
            hand_card_id = gDuel_aActiveCards[hand_index + AI_SLOT_OWN_HAND_FIRST].card_id;
            hand_slot_index = hand_index + AI_SLOT_OWN_HAND_FIRST;
            if (hand_card_id != 0) {
                if (Ai_IsCardInSets(gAiScript_State.fusion_set, hand_slot_index) == 0) {
                    result_card_id = Duel_CheckFusion(candidate_card_id, hand_card_id);
                    if (result_card_id == 0)
                        result_card_id = Duel_CheckEquip(candidate_card_id, hand_card_id);
                    if (result_card_id != 0) {
                        gAiScript_State.fusion_path[gAiScript_State.fusion_depth] = hand_slot_index;
                        if (
                            Duel_GetBaseCardStat(result_card_id, 0) >
                                gAiScript_State.fusion_best_stat ||
                            (
                                Duel_GetBaseCardStat(result_card_id, 0) ==
                                    gAiScript_State.fusion_best_stat &&
                                gAiScript_State.fusion_depth <
                                    gAiScript_State.fusion_best_depth
                            )
                        ) {
                            gAiScript_State.fusion_best_stat =
                                Duel_GetBaseCardStat(result_card_id, 0);
                            gAiScript_State.fusion_best_depth =
                                gAiScript_State.fusion_depth;
                            for (path_index = 0;
                                 path_index <= (s32)gAiScript_State.fusion_best_depth;
                                 path_index++)
                                gAiScript_State.combo_cards[path_index] =
                                    gAiScript_State.fusion_path[path_index];
                            gAiScript_State.combo_cards[path_index] = 0;
                        }
                        if (
                            Duel_GetBaseCardStat(result_card_id, 1) >
                                gAiScript_State.fusion_best_stat ||
                            (
                                Duel_GetBaseCardStat(result_card_id, 1) ==
                                    gAiScript_State.fusion_best_stat &&
                                gAiScript_State.fusion_depth <
                                    gAiScript_State.fusion_best_depth
                            )
                        ) {
                            gAiScript_State.fusion_best_stat =
                                Duel_GetBaseCardStat(result_card_id, 1);
                            gAiScript_State.fusion_best_depth =
                                gAiScript_State.fusion_depth;
                            for (path_index = 0;
                                 path_index <= (s32)gAiScript_State.fusion_best_depth;
                                 path_index++)
                                gAiScript_State.combo_cards[path_index] =
                                    gAiScript_State.fusion_path[path_index];
                            gAiScript_State.combo_cards[path_index] = 0;
                        }
                        if (gAiScript_State.fusion_depth <
                            gAiScript_State.fusion_limit - 1) {
                            gAiScript_State.fusion_used[hand_index] = 1;
                            gAiScript_State.fusion_depth++;
                            Ai_CompleteFusion(result_card_id);
                            gAiScript_State.fusion_used[hand_index] = 0;
                            gAiScript_State.fusion_depth--;
                        }
                    }
                }
            }
        }
        hand_index++;
    } while (hand_index < gAiScript_State.fusion_count);
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
    s32 depth_limit_work;
    s32 excluded_set;
    s32 result_register;
    s32 hand_search_count;
    s32 search_index;
    s32 candidate_slot;
    s32 candidate_card_id;
    u8 *state_byte_cursor;
    s32 *script_memory;

    script_memory = gAiScript_aMemory;
    depth_limit_work = script_memory[AiScript_ReadByte()];
    excluded_set = script_memory[AiScript_ReadByte()];
    depth_limit_work = depth_limit_work + 1;
    result_register = AiScript_ReadByte();
    hand_search_count = Ai_GetHandSearchCount();

    gAiScript_State.fusion_count = hand_search_count;
    gAiScript_State.fusion_best_stat = 0;
    gAiScript_State.fusion_depth = 0;
    gAiScript_State.fusion_limit = depth_limit_work;
    gAiScript_State.fusion_set = excluded_set;

    for (search_index = 0; search_index < gAiScript_State.fusion_count; search_index++) {
        gAiScript_State.fusion_used[search_index] = 0;
    }

    for (search_index = 0; search_index < DUEL_FIELD_ROW_SIZE; search_index++) {
        candidate_card_id = gDuel_aActiveCards[search_index + AI_SLOT_OWN_MONSTER_FIRST].card_id;
        candidate_slot = search_index + 1;
        if (candidate_card_id == 0) {
            continue;
        }
        if (Ai_IsCardInSets(excluded_set, candidate_slot) != 0) {
            continue;
        }
        gAiScript_State.fusion_path[gAiScript_State.fusion_depth] = candidate_slot;
        if (gDuel_aActiveCards[search_index + AI_SLOT_OWN_MONSTER_FIRST].attack >
            gAiScript_State.fusion_best_stat) {
            gAiScript_State.fusion_best_stat =
                gDuel_aActiveCards[search_index + AI_SLOT_OWN_MONSTER_FIRST].attack;
            gAiScript_State.combo_cards[0] = candidate_slot;
            gAiScript_State.combo_cards[1] = 0;
            gAiScript_State.fusion_best_depth =
                gAiScript_State.fusion_depth;
        }
        if (gDuel_aActiveCards[search_index + AI_SLOT_OWN_MONSTER_FIRST].defense >
            gAiScript_State.fusion_best_stat) {
            gAiScript_State.fusion_best_stat =
                gDuel_aActiveCards[search_index + AI_SLOT_OWN_MONSTER_FIRST].defense;
            gAiScript_State.combo_cards[0] = candidate_slot;
            gAiScript_State.combo_cards[1] = 0;
            gAiScript_State.fusion_best_depth =
                gAiScript_State.fusion_depth;
        }
        if (gAiScript_State.fusion_limit >= 2) {
            /* state_byte_cursor deliberately points at the state base plus search_index, not at
             * &fusion_used[search_index]: keeping fusion_used's 0xAA in the access
             * displacement lets the address arithmetic fill the branch delay
             * slot. Spelling it &fusion_used[search_index] costs two instructions. */
            state_byte_cursor = (u8 *)&gAiScript_State + search_index;
            state_byte_cursor[AI_SCRIPT_FUSION_USED_BYTE_OFFSET] = 1;
            gAiScript_State.fusion_depth =
                gAiScript_State.fusion_depth + 1;
            Ai_CompleteFusion(candidate_card_id);
            state_byte_cursor[AI_SCRIPT_FUSION_USED_BYTE_OFFSET] = 0;
            gAiScript_State.fusion_depth =
                gAiScript_State.fusion_depth - 1;
        }
    }

    for (search_index = 0; search_index < gAiScript_State.fusion_count; search_index++) {
        candidate_card_id = gDuel_aActiveCards[search_index + AI_SLOT_OWN_HAND_FIRST].card_id;
        candidate_slot = search_index + AI_SLOT_OWN_HAND_FIRST;
        if (candidate_card_id == 0) {
            continue;
        }
        if (Ai_IsCardInSets(excluded_set, candidate_slot) != 0) {
            continue;
        }
        gAiScript_State.fusion_path[gAiScript_State.fusion_depth] = candidate_slot;
        if (gDuel_aActiveCards[search_index + AI_SLOT_OWN_HAND_FIRST].attack >
            gAiScript_State.fusion_best_stat) {
            gAiScript_State.fusion_best_stat =
                gDuel_aActiveCards[search_index + AI_SLOT_OWN_HAND_FIRST].attack;
            gAiScript_State.combo_cards[0] = candidate_slot;
            gAiScript_State.combo_cards[1] = 0;
            gAiScript_State.fusion_best_depth =
                gAiScript_State.fusion_depth;
        }
        if (gDuel_aActiveCards[search_index + AI_SLOT_OWN_HAND_FIRST].defense >
            gAiScript_State.fusion_best_stat) {
            gAiScript_State.fusion_best_stat =
                gDuel_aActiveCards[search_index + AI_SLOT_OWN_HAND_FIRST].defense;
            gAiScript_State.combo_cards[0] = candidate_slot;
            gAiScript_State.combo_cards[1] = 0;
            gAiScript_State.fusion_best_depth =
                gAiScript_State.fusion_depth;
        }
        if (gAiScript_State.fusion_limit >= 3) {
            /* state_byte_cursor deliberately points at the state base plus search_index, not at
             * &fusion_used[search_index]: keeping fusion_used's 0xAA in the access
             * displacement lets the address arithmetic fill the branch delay
             * slot. Spelling it &fusion_used[search_index] costs two instructions. */
            state_byte_cursor = (u8 *)&gAiScript_State + search_index;
            state_byte_cursor[AI_SCRIPT_FUSION_USED_BYTE_OFFSET] = 1;
            gAiScript_State.fusion_depth =
                gAiScript_State.fusion_depth + 1;
            Ai_CompleteFusion(candidate_card_id);
            state_byte_cursor[AI_SCRIPT_FUSION_USED_BYTE_OFFSET] = 0;
            gAiScript_State.fusion_depth =
                gAiScript_State.fusion_depth - 1;
        }
    }

    if (gAiScript_State.combo_cards[0] != 0) {
        if (gAiScript_State.combo_cards[0] >= DUEL_FIELD_ROW_SIZE + 1) {
            gAiScript_aMemory[result_register] = 0;
        } else if (gAiScript_State.combo_cards[1] != 0) {
            gAiScript_aMemory[result_register] = 1;
        } else {
            gAiScript_aMemory[result_register] = 2;
        }
    } else {
        gAiScript_aMemory[result_register] = 3;
    }
}

void AiScript_EvaluateFusion(void)
{
    s32 starting_card_id;
    s32 depth_limit;
    s32 excluded_set;
    s32 result_register;
    s32 attack_stat;
    s32 defense_stat;
    s32 hand_index;
    s32 hand_search_count;

    starting_card_id = gAiScript_aMemory[AiScript_ReadByte()];
    depth_limit = gAiScript_aMemory[AiScript_ReadByte()] + 1;
    excluded_set = gAiScript_aMemory[AiScript_ReadByte()];
    result_register = AiScript_ReadByte();
    hand_search_count = Ai_GetHandSearchCount();

    gAiScript_State.fusion_count = hand_search_count;
    gAiScript_State.fusion_limit = depth_limit;
    gAiScript_State.fusion_best_stat = 0;
    gAiScript_State.fusion_depth = 0;
    gAiScript_State.fusion_best_depth = 0;
    gAiScript_State.fusion_set = excluded_set;

    for (hand_index = 0; hand_index < gAiScript_State.fusion_count; hand_index++) {
        gAiScript_State.fusion_used[hand_index] = 0;
    }

    attack_stat = Duel_GetBaseCardStat(starting_card_id, 0);
    defense_stat = Duel_GetBaseCardStat(starting_card_id, 1);

    if (defense_stat < attack_stat) {
        gAiScript_State.fusion_best_stat = Duel_GetBaseCardStat(starting_card_id, 0);
    } else {
        gAiScript_State.fusion_best_stat = Duel_GetBaseCardStat(starting_card_id, 1);
    }

    Ai_CompleteFusion(starting_card_id);

    if (gAiScript_State.fusion_best_depth != 0) {
        gAiScript_aMemory[result_register] = 0;
    } else {
        gAiScript_aMemory[result_register] = 1;
    }
}

/* The European image has no skip handlers; its script table points at the
   two empty handlers below instead. */
#ifndef VERSION_EUROPE
void AiScript_SkipHand(void)
{
    AiScript_ReadByte();
    AiScript_ReadByte();
    AiScript_ReadByte();
    AiScript_ReadByte();
}

void AiScript_SkipField(void)
{
    AiScript_ReadByte();
    AiScript_ReadByte();
    AiScript_ReadByte();
    AiScript_ReadByte();
}
#endif

#ifdef VERSION_EUROPE
/* Two empty European-only script handlers, right before AiScript_PlayFaceUp
   in the image; the European AI script table points at both. */
void func_800732E0(void)
{
}

void func_800732E8(void)
{
}
#endif

void AiScript_PlayFaceUp(void)
{
    s32 *memory = gAiScript_aMemory;
    s32 a = memory[AiScript_ReadByte()];
    s32 b = memory[AiScript_ReadByte()];
    s32 c = memory[AiScript_ReadByte()];
    s32 d = memory[AiScript_ReadByte()];
    s32 e = memory[AiScript_ReadByte()];

    gAi_Selection.bytes[AI_SCRIPT_COMBO_CARD_COUNT - 1] = 0;
    gAi_Selection.bytes[0] = a;
    gAi_Selection.bytes[1] = b;
    gAi_Selection.bytes[2] = c;
    gAi_Selection.bytes[3] = d;
    gAi_Selection.bytes[4] = e;
}

void AiScript_SetPosition(void)
{
    s32 *memory = gAiScript_aMemory;
    s32 value;

    value = memory[AiScript_ReadByte()];
    D_800EAE8E[0] = value;
}
