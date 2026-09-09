#include "../types.h"
#include "func_8002CBF4.h"
#include "ai.h"
#include "card_constants.h"
#include "duel_check_ritual.h"
#include "duel_card.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"

extern u8 D_800EAE88[];
extern s32 Duel_CheckEquip(s32, s32);
extern s32 Duel_CheckFusion(s32, s32);

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
