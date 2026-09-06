#ifndef YUGIOH_GAME_AI_H
#define YUGIOH_GAME_AI_H

#include "../types.h"

#define AI_SCRIPT_CARD_SET_COUNT 32
#define AI_SCRIPT_COMBO_CARD_COUNT 6
#define AI_SCRIPT_PERCENT_SCALE 100
#define AI_SCRIPT_MEMORY_COUNT 20
#define AI_SCRIPT_RETURN_STACK_COUNT 8
#define AI_SCRIPT_TYPE_SET_COUNT 25

typedef void (*AiScriptHandler)(void);

typedef struct {
    s16 card_id;
    u8 pad02[4];
    u16 flags;
    s8 card_type;
    u8 pad09[3];
} AiActiveCard;

typedef struct {
    u8 enabled;
    u8 pad01[3];
    u8 *script_base;
    u8 *script_cursor;
    u8 *previous_cursor;
    u8 pad10[4];
    u8 return_depth;
    u8 pad15[3];
    u8 *return_stack[AI_SCRIPT_RETURN_STACK_COUNT];
    u8 combo_cards[AI_SCRIPT_COMBO_CARD_COUNT];
    u16 card_set[AI_SCRIPT_CARD_SET_COUNT];
    u8 type_set[AI_SCRIPT_TYPE_SET_COUNT];
    u8 pad97[5];
    u8 fusion_count;
    u8 fusion_limit;
    u8 fusion_set;
    u8 pad9F;
    u16 fusion_best_stat;
    u8 fusion_depth;
    u8 fusion_best_depth;
    u8 fusion_path[6];
    u8 fusion_used[0x2A];
} AiScriptState;

typedef struct {
    u8 pad00[20];
    s16 life_points;
    u8 pad16[3];
    s8 pinned;
    u8 pad1A[6];
} AiDuelistState;

typedef struct {
    u8 pad_00[9];
    s8 field_09;
    s8 field_0A;
    s8 field_0B;
} AiFieldCardState;

typedef char AiActiveCard_size_must_be_0x0C[
    sizeof(AiActiveCard) == 0x0C ? 1 : -1
];
typedef char AiScriptState_size_must_be_0xD4[
    sizeof(AiScriptState) == 0xD4 ? 1 : -1
];
typedef char AiDuelistState_size_must_be_0x20[
    sizeof(AiDuelistState) == 0x20 ? 1 : -1
];
typedef char AiFieldCardState_size_must_be_0x0C[
    sizeof(AiFieldCardState) == 0x0C ? 1 : -1
];

extern s32 gAiScript_aMemory[AI_SCRIPT_MEMORY_COUNT];
extern AiScriptHandler gAiScript_apfnCommand[];
extern AiActiveCard gDuel_aActiveCards[];
extern AiDuelistState D_800E9FF0[];

#endif
