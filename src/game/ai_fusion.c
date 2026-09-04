#include "../types.h"
#include "ai.h"
#include "card_constants.h"

extern u8 D_800EAE88[];
extern u8 gAiScript_State[];

extern s32 AiScript_ReadByte(void);
extern s32 Ai_IsCardInSets(s32, s32);
extern s32 Duel_CheckEquip(s32, s32);
extern s32 Duel_CheckFusion(s32, s32);
extern s32 Duel_CheckRitual(s32, s32);
extern s32 Duel_GetBaseCardStat();

void AiScript_FindEquipTarget(void)
{
    s32 owner = gAiScript_aMemory[AiScript_ReadByte()];
    s32 mode = gAiScript_aMemory[AiScript_ReadByte()];
    s32 type = gAiScript_aMemory[AiScript_ReadByte()];
    s32 dest = AiScript_ReadByte();
    s32 i;

    for (i = 1; i < 6; i++) {
        AiActiveCard *card = &gDuel_aActiveCards[i];

        if (!card->card_id)
            continue;
        if (mode == 1 && (card->flags & 0x4000))
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

    for (i = 0; i < 5; i++) {
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
    } while (i < 5);

    gAiScript_aMemory[dest] = 0;
}

void Ai_CompleteFusion(s32 arg0)
{
    u8 *state;
    u8 *cursor;
    s32 i;
    s32 index;
    s32 card_id;
    s32 result;
    s32 j;

    state = gAiScript_State;
    i = 0;
    if (state[0x9C] == 0)
        return;
    cursor = state;
    do {
        if (cursor[i + 0xAA] == 0) {
            card_id = gDuel_aActiveCards[i + 0xB].card_id;
            index = i + 0xB;
            if (card_id != 0) {
                if (Ai_IsCardInSets(cursor[0x9E], index) == 0) {
                    result = Duel_CheckFusion(arg0, card_id);
                    if (result == 0)
                        result = Duel_CheckEquip(arg0, card_id);
                    if (result != 0) {
                        cursor[cursor[0xA2] + 0xA4] = index;
                        if (
                            Duel_GetBaseCardStat(result, 0) >
                                *(u16 *)(cursor + 0xA0) ||
                            (
                                Duel_GetBaseCardStat(result, 0) ==
                                    *(u16 *)(cursor + 0xA0) &&
                                cursor[0xA2] < cursor[0xA3]
                            )
                        ) {
                            *(u16 *)(cursor + 0xA0) =
                                Duel_GetBaseCardStat(result, 0);
                            cursor[0xA3] = cursor[0xA2];
                            for (j = 0; j <= (s32)cursor[0xA3]; j++)
                                cursor[j + 0x38] = cursor[j + 0xA4];
                            cursor[j + 0x38] = 0;
                        }
                        if (
                            Duel_GetBaseCardStat(result, 1) >
                                *(u16 *)(cursor + 0xA0) ||
                            (
                                Duel_GetBaseCardStat(result, 1) ==
                                    *(u16 *)(cursor + 0xA0) &&
                                cursor[0xA2] < cursor[0xA3]
                            )
                        ) {
                            *(u16 *)(cursor + 0xA0) =
                                Duel_GetBaseCardStat(result, 1);
                            cursor[0xA3] = cursor[0xA2];
                            for (j = 0; j <= (s32)cursor[0xA3]; j++)
                                cursor[j + 0x38] = cursor[j + 0xA4];
                            cursor[j + 0x38] = 0;
                        }
                        if (cursor[0xA2] < cursor[0x9D] - 1) {
                            cursor[i + 0xAA] = 1;
                            cursor[0xA2]++;
                            Ai_CompleteFusion(result);
                            cursor[i + 0xAA] = 0;
                            cursor[0xA2]--;
                        }
                    }
                }
            }
        }
        i++;
    } while (i < cursor[0x9C]);
}
