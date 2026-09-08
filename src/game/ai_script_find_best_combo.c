#include "../types.h"
#include "ai_script_read_byte.h"
#include "ai_constants.h"
#include "duel_grid.h"

struct ActiveCardEntry {
    s16 card_id;
    s16 attack;
    s16 defense;
    u16 flags;
    u8 pad_08[4];
};

extern s32 gAiScript_aMemory[];
extern u8 gAiScript_State[];
extern struct ActiveCardEntry gDuel_aActiveCards[];

extern s32 Ai_GetHandSize(void);
extern s32 Ai_IsCardInSets(s32, s32);
extern void Ai_CompleteFusion(s32);

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

    gAiScript_State[AI_SCRIPT_FUSION_COUNT_BYTE_OFFSET] = n;
    *(s16 *)(gAiScript_State + AI_SCRIPT_FUSION_BEST_STAT_BYTE_OFFSET) = 0;
    gAiScript_State[AI_SCRIPT_FUSION_DEPTH_BYTE_OFFSET] = 0;
    gAiScript_State[AI_SCRIPT_FUSION_LIMIT_BYTE_OFFSET] = depth;
    gAiScript_State[AI_SCRIPT_FUSION_SET_BYTE_OFFSET] = sets;

    for (i = 0; i < gAiScript_State[AI_SCRIPT_FUSION_COUNT_BYTE_OFFSET]; i++) {
        gAiScript_State[i + AI_SCRIPT_FUSION_USED_BYTE_OFFSET] = 0;
    }

    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
        card = gDuel_aActiveCards[i + 1].card_id;
        slot = i + 1;
        if (card == 0) {
            continue;
        }
        if (Ai_IsCardInSets(sets, slot) != 0) {
            continue;
        }
        gAiScript_State[gAiScript_State[AI_SCRIPT_FUSION_DEPTH_BYTE_OFFSET] +
                        AI_SCRIPT_FUSION_PATH_BYTE_OFFSET] = slot;
        if (gDuel_aActiveCards[i + 1].attack >
            *(u16 *)(gAiScript_State + AI_SCRIPT_FUSION_BEST_STAT_BYTE_OFFSET)) {
            *(u16 *)(gAiScript_State + AI_SCRIPT_FUSION_BEST_STAT_BYTE_OFFSET) =
                gDuel_aActiveCards[i + 1].attack;
            gAiScript_State[AI_SCRIPT_COMBO_BYTE_OFFSET] = slot;
            gAiScript_State[AI_SCRIPT_COMBO_BYTE_OFFSET + 1] = 0;
            gAiScript_State[AI_SCRIPT_FUSION_BEST_DEPTH_BYTE_OFFSET] =
                gAiScript_State[AI_SCRIPT_FUSION_DEPTH_BYTE_OFFSET];
        }
        if (gDuel_aActiveCards[i + 1].defense >
            *(u16 *)(gAiScript_State + AI_SCRIPT_FUSION_BEST_STAT_BYTE_OFFSET)) {
            *(u16 *)(gAiScript_State + AI_SCRIPT_FUSION_BEST_STAT_BYTE_OFFSET) =
                gDuel_aActiveCards[i + 1].defense;
            gAiScript_State[AI_SCRIPT_COMBO_BYTE_OFFSET] = slot;
            gAiScript_State[AI_SCRIPT_COMBO_BYTE_OFFSET + 1] = 0;
            gAiScript_State[AI_SCRIPT_FUSION_BEST_DEPTH_BYTE_OFFSET] =
                gAiScript_State[AI_SCRIPT_FUSION_DEPTH_BYTE_OFFSET];
        }
        if (gAiScript_State[AI_SCRIPT_FUSION_LIMIT_BYTE_OFFSET] >= 2) {
            e = &gAiScript_State[i];
            e[AI_SCRIPT_FUSION_USED_BYTE_OFFSET] = 1;
            gAiScript_State[AI_SCRIPT_FUSION_DEPTH_BYTE_OFFSET] =
                gAiScript_State[AI_SCRIPT_FUSION_DEPTH_BYTE_OFFSET] + 1;
            Ai_CompleteFusion(card);
            e[AI_SCRIPT_FUSION_USED_BYTE_OFFSET] = 0;
            gAiScript_State[AI_SCRIPT_FUSION_DEPTH_BYTE_OFFSET] =
                gAiScript_State[AI_SCRIPT_FUSION_DEPTH_BYTE_OFFSET] - 1;
        }
    }

    for (i = 0; i < gAiScript_State[AI_SCRIPT_FUSION_COUNT_BYTE_OFFSET]; i++) {
        card = gDuel_aActiveCards[i + 0xB].card_id;
        slot = i + 0xB;
        if (card == 0) {
            continue;
        }
        if (Ai_IsCardInSets(sets, slot) != 0) {
            continue;
        }
        gAiScript_State[gAiScript_State[AI_SCRIPT_FUSION_DEPTH_BYTE_OFFSET] +
                        AI_SCRIPT_FUSION_PATH_BYTE_OFFSET] = slot;
        if (gDuel_aActiveCards[i + 0xB].attack >
            *(u16 *)(gAiScript_State + AI_SCRIPT_FUSION_BEST_STAT_BYTE_OFFSET)) {
            *(u16 *)(gAiScript_State + AI_SCRIPT_FUSION_BEST_STAT_BYTE_OFFSET) =
                gDuel_aActiveCards[i + 0xB].attack;
            gAiScript_State[AI_SCRIPT_COMBO_BYTE_OFFSET] = slot;
            gAiScript_State[AI_SCRIPT_COMBO_BYTE_OFFSET + 1] = 0;
            gAiScript_State[AI_SCRIPT_FUSION_BEST_DEPTH_BYTE_OFFSET] =
                gAiScript_State[AI_SCRIPT_FUSION_DEPTH_BYTE_OFFSET];
        }
        if (gDuel_aActiveCards[i + 0xB].defense >
            *(u16 *)(gAiScript_State + AI_SCRIPT_FUSION_BEST_STAT_BYTE_OFFSET)) {
            *(u16 *)(gAiScript_State + AI_SCRIPT_FUSION_BEST_STAT_BYTE_OFFSET) =
                gDuel_aActiveCards[i + 0xB].defense;
            gAiScript_State[AI_SCRIPT_COMBO_BYTE_OFFSET] = slot;
            gAiScript_State[AI_SCRIPT_COMBO_BYTE_OFFSET + 1] = 0;
            gAiScript_State[AI_SCRIPT_FUSION_BEST_DEPTH_BYTE_OFFSET] =
                gAiScript_State[AI_SCRIPT_FUSION_DEPTH_BYTE_OFFSET];
        }
        if (gAiScript_State[AI_SCRIPT_FUSION_LIMIT_BYTE_OFFSET] >= 3) {
            e = &gAiScript_State[i];
            e[AI_SCRIPT_FUSION_USED_BYTE_OFFSET] = 1;
            gAiScript_State[AI_SCRIPT_FUSION_DEPTH_BYTE_OFFSET] =
                gAiScript_State[AI_SCRIPT_FUSION_DEPTH_BYTE_OFFSET] + 1;
            Ai_CompleteFusion(card);
            e[AI_SCRIPT_FUSION_USED_BYTE_OFFSET] = 0;
            gAiScript_State[AI_SCRIPT_FUSION_DEPTH_BYTE_OFFSET] =
                gAiScript_State[AI_SCRIPT_FUSION_DEPTH_BYTE_OFFSET] - 1;
        }
    }

    if (gAiScript_State[AI_SCRIPT_COMBO_BYTE_OFFSET] != 0) {
        if (gAiScript_State[AI_SCRIPT_COMBO_BYTE_OFFSET] >= DUEL_FIELD_ROW_SIZE + 1) {
            gAiScript_aMemory[dest] = 0;
        } else if (gAiScript_State[AI_SCRIPT_COMBO_BYTE_OFFSET + 1] != 0) {
            gAiScript_aMemory[dest] = 1;
        } else {
            gAiScript_aMemory[dest] = 2;
        }
    } else {
        gAiScript_aMemory[dest] = 3;
    }
}
