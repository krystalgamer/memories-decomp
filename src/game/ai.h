#ifndef YUGIOH_GAME_AI_H
#define YUGIOH_GAME_AI_H

#include "../types.h"
#include "ai_constants.h"
#include "duel_grid.h"

#define AI_SCRIPT_STATE_OFFSET(type, member) ((u32)&(((type *)0)->member))

typedef void (*AiScriptHandler)(void);

typedef struct {
    s16 card_id;
    s16 attack;
    s16 defense;
    u16 flags;
    s8 card_type;
    /* AiScript_FindKiller and func_8007308C both read +0x09 and pass it
     * to Duel_CalcGuardianStarMatchup; both match byte for byte. */
    s8 guardian_star;
    s8 guardian_star_2;
    u8 deck_index;
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
    /* func_80073464 and func_80073474 set and clear this byte as a paired
     * flag, so 0x97 is live state rather than padding before the attack
     * scratch. Its meaning is not otherwise evidenced, so it keeps an
     * address-based name. */
    u8 field_97;
    /* AiScript_FindBestAttack zeroes 0x98 on entry, then keeps a running
     * maximum there (compared unsigned, stored signed) and records the pair
     * of slots that produced it: the attacking slot i at 0x9A and the target
     * slot j at 0x9B. Nothing else in the image touches these three. */
    u16 attack_best_stat;
    s8 attack_best_slot;
    s8 attack_best_target;
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

/* The AI script VM's register file. Nineteen sources under ai_script_* and
   ai_* reach it, every one of them with the identical declaration this
   replaces; nothing in C defines it, so it is still generated data. */
extern AiScriptState gAiScript_State;

/* The AI's pending selection at D_800EAE88, written three ways by different
 * callers: ai_script_actions.c fills 0x00..0x05 as a combo list, the turn
 * logic writes the slot/flag bytes at 0x00, 0x01, 0x06, 0x07 and 0x08, and
 * the field-card path writes 0x09..0x0B. Previously that was split across
 * two competing typedefs -- this one in the header covering 0x09..0x0B, and
 * a second inside ai_turn_action.c covering 0x00..0x08 -- describing the
 * same twelve bytes. */
typedef struct {
    s8 result;      /* 0x00 */
    s8 field1;      /* 0x01 */
    char pad_02[4]; /* 0x02 */
    s8 value;       /* 0x06 */
    s8 zero;        /* 0x07 */
    s8 random;      /* 0x08 */
    s8 field_09;    /* 0x09 */
    s8 field_0A;    /* 0x0A */
    s8 field_0B;    /* 0x0B */
} AiSelection;

typedef char AiActiveCard_size_must_be_0x0C[
    sizeof(AiActiveCard) == AI_ACTIVE_CARD_RECORD_SIZE ? 1 : -1
];
typedef char AiActiveCard_row_slot_count_must_be_the_field_row_size[
    AI_ACTIVE_CARD_ROW_SLOT_COUNT == DUEL_FIELD_ROW_SIZE ? 1 : -1
];
typedef char AiActiveCard_opponent_hand_must_start_at_slot_66[
    AI_SLOT_OPPONENT_HAND_FIRST == 66 ? 1 : -1
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
typedef char AiScriptState_field_97_offset_must_be_0x97[
    AI_SCRIPT_STATE_OFFSET(AiScriptState, field_97) == 0x97 ? 1 : -1
];
typedef char AiScriptState_attack_best_stat_offset_must_be_0x98[
    AI_SCRIPT_STATE_OFFSET(AiScriptState, attack_best_stat) == 0x98 ? 1 : -1
];
typedef char AiScriptState_attack_best_slot_offset_must_be_0x9A[
    AI_SCRIPT_STATE_OFFSET(AiScriptState, attack_best_slot) == 0x9A ? 1 : -1
];
typedef char AiScriptState_attack_best_target_offset_must_be_0x9B[
    AI_SCRIPT_STATE_OFFSET(AiScriptState, attack_best_target) == 0x9B ? 1 : -1
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
typedef char AiSelection_size_must_be_0x0C[
    sizeof(AiSelection) == 0x0C ? 1 : -1
];
typedef char AiSelection_value_offset_must_be_0x06[
    AI_SCRIPT_STATE_OFFSET(AiSelection, value) == 0x06 ? 1 : -1
];
typedef char AiSelection_zero_offset_must_be_0x07[
    AI_SCRIPT_STATE_OFFSET(AiSelection, zero) == 0x07 ? 1 : -1
];
typedef char AiSelection_random_offset_must_be_0x08[
    AI_SCRIPT_STATE_OFFSET(AiSelection, random) == 0x08 ? 1 : -1
];
typedef char AiSelection_field_09_offset_must_be_0x09[
    AI_SCRIPT_STATE_OFFSET(AiSelection, field_09) == 0x09 ? 1 : -1
];

#undef AI_SCRIPT_STATE_OFFSET

extern s32 gAiScript_aMemory[AI_SCRIPT_MEMORY_COUNT];
extern AiScriptHandler gAiScript_apfnCommand[];
extern AiActiveCard gDuel_aActiveCards[];

s32 Ai_IsCardInSets(s32 mode, s32 index);
void Ai_GetWinningCardRange(s32 kind, s32 *low, s32 *high);
void Ai_GetCardRange(s32 kind, s32 *low, s32 *high);
/* ai_card_ranges.c needs the historical widened return declaration; using
 * the definition's s8 type there adds four sign-extension instructions. */
#ifdef AI_HAND_SIZE_RETURNS_S32
s32 Ai_GetHandSize(void);
#else
s8 Ai_GetHandSize(void);
#endif

#endif
