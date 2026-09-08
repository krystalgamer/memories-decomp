#ifndef YUGIOH_GAME_AI_H
#define YUGIOH_GAME_AI_H

#include "../types.h"
#include "ai_constants.h"

#define AI_SCRIPT_STATE_OFFSET(type, member) ((u32)&(((type *)0)->member))

typedef void (*AiScriptHandler)(void);

typedef struct {
    s16 card_id;
    s16 attack;
    s16 defense;
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
    /* func_80070DA8 pops `script_cursor = return_stack[--return_depth]` and
     * prints "ERROR:Can't Return From Routine" when the depth is already
     * zero. */
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
    sizeof(AiActiveCard) == AI_ACTIVE_CARD_RECORD_SIZE ? 1 : -1
];
typedef char AiActiveCard_side_stride_must_be_55_records[
    AI_ACTIVE_CARD_SIDE_BYTE_STRIDE == 55 * sizeof(AiActiveCard) ? 1 : -1
];
typedef char AiScriptState_size_must_be_0xD4[
    sizeof(AiScriptState) == 0xD4 ? 1 : -1
];
typedef char AiScriptState_card_set_offset_must_be_0x3E[
    AI_SCRIPT_STATE_OFFSET(AiScriptState, card_set) ==
        AI_SCRIPT_CARD_SET_BYTE_OFFSET ? 1 : -1
];
typedef char AiScriptState_card_set_entry_size_must_be_2[
    sizeof(((AiScriptState *)0)->card_set[0]) ==
        AI_SCRIPT_CARD_SET_ENTRY_SIZE ? 1 : -1
];
typedef char AiScriptState_type_set_offset_must_be_0x7E[
    AI_SCRIPT_STATE_OFFSET(AiScriptState, type_set) ==
        AI_SCRIPT_TYPE_SET_BYTE_OFFSET ? 1 : -1
];
typedef char AiScriptState_combo_offset_must_be_0x38[
    AI_SCRIPT_STATE_OFFSET(AiScriptState, combo_cards) ==
        AI_SCRIPT_COMBO_BYTE_OFFSET ? 1 : -1
];
typedef char AiScriptState_fusion_count_offset_must_be_0x9C[
    AI_SCRIPT_STATE_OFFSET(AiScriptState, fusion_count) ==
        AI_SCRIPT_FUSION_COUNT_BYTE_OFFSET ? 1 : -1
];
typedef char AiScriptState_fusion_limit_offset_must_be_0x9D[
    AI_SCRIPT_STATE_OFFSET(AiScriptState, fusion_limit) ==
        AI_SCRIPT_FUSION_LIMIT_BYTE_OFFSET ? 1 : -1
];
typedef char AiScriptState_fusion_set_offset_must_be_0x9E[
    AI_SCRIPT_STATE_OFFSET(AiScriptState, fusion_set) ==
        AI_SCRIPT_FUSION_SET_BYTE_OFFSET ? 1 : -1
];
typedef char AiScriptState_fusion_best_stat_offset_must_be_0xA0[
    AI_SCRIPT_STATE_OFFSET(AiScriptState, fusion_best_stat) ==
        AI_SCRIPT_FUSION_BEST_STAT_BYTE_OFFSET ? 1 : -1
];
typedef char AiScriptState_fusion_depth_offset_must_be_0xA2[
    AI_SCRIPT_STATE_OFFSET(AiScriptState, fusion_depth) ==
        AI_SCRIPT_FUSION_DEPTH_BYTE_OFFSET ? 1 : -1
];
typedef char AiScriptState_fusion_best_depth_offset_must_be_0xA3[
    AI_SCRIPT_STATE_OFFSET(AiScriptState, fusion_best_depth) ==
        AI_SCRIPT_FUSION_BEST_DEPTH_BYTE_OFFSET ? 1 : -1
];
typedef char AiScriptState_fusion_path_offset_must_be_0xA4[
    AI_SCRIPT_STATE_OFFSET(AiScriptState, fusion_path) ==
        AI_SCRIPT_FUSION_PATH_BYTE_OFFSET ? 1 : -1
];
typedef char AiScriptState_fusion_used_offset_must_be_0xAA[
    AI_SCRIPT_STATE_OFFSET(AiScriptState, fusion_used) ==
        AI_SCRIPT_FUSION_USED_BYTE_OFFSET ? 1 : -1
];
typedef char AiDuelistState_size_must_be_0x20[
    sizeof(AiDuelistState) == 0x20 ? 1 : -1
];
typedef char AiFieldCardState_size_must_be_0x0C[
    sizeof(AiFieldCardState) == 0x0C ? 1 : -1
];

#undef AI_SCRIPT_STATE_OFFSET

extern s32 gAiScript_aMemory[AI_SCRIPT_MEMORY_COUNT];
extern AiScriptHandler gAiScript_apfnCommand[];
extern AiActiveCard gDuel_aActiveCards[];
extern AiDuelistState D_800E9FF0[];

#endif
