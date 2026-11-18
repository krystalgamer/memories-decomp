#ifndef YUGIOH_GAME_AI_H
#define YUGIOH_GAME_AI_H

#include "../types.h"
#include "ai_constants.h"
#include "duel_grid.h"

typedef void (*AiScriptHandler)(void);

typedef struct {
    s16 card_id;
    s16 attack;
    s16 defense;
    u16 flags;
    s8 card_type;
    /* AiScript_FindKiller and AiScript_FindBestAttack both read +0x09 and pass it
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
    /* AiScript_Return pops `script_cursor = return_stack[--return_depth]` and
     * prints "ERROR:Can't Return From Routine" when the depth is already
     * zero. */
    u8 *return_stack[AI_SCRIPT_RETURN_STACK_COUNT];
    u8 combo_cards[AI_SCRIPT_COMBO_CARD_COUNT];
    u16 card_set[AI_SCRIPT_CARD_SET_COUNT];
    u8 type_set[AI_SCRIPT_TYPE_SET_COUNT];
    /* Script opcodes 0x3B and 0x3C (AiScript_SetUnreadFlag and
     * AiScript_ClearUnreadFlag) set and clear this byte as a paired flag, so
     * 0x97 is live state rather than padding before the attack scratch.
     * Nothing in the image reads it, and its purpose is not evidenced. */
    u8 unread_flag;
    /* AiScript_FindBestAttack zeroes 0x98 on entry, then keeps a running
     * maximum there (compared unsigned, stored signed) and records the pair
     * of slots that produced it: the attacking slot i at 0x9A and the target
     * slot j at 0x9B. AiScript_LoadBestDifference, AiScript_LoadBestAttacker
     * and AiScript_LoadBestTarget read all three back unsigned into script
     * memory; nothing else in the image touches them. */
    u16 attack_best_stat;
    u8 attack_best_slot;
    u8 attack_best_target;
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

/* The AI's pending selection at gAi_Selection, written three ways by different
 * callers: AiScript_PlayFaceUp (ai_script.c) fills 0x00..0x05 as a combo list, the turn
 * logic writes the slot/flag bytes at 0x00, 0x01, 0x06, 0x07 and 0x08, and
 * the field-card path writes 0x09..0x0B. Previously that was split across
 * two competing typedefs -- this one in the header covering 0x09..0x0B, and
 * a second inside duel_scene.c covering 0x00..0x08 -- describing the
 * same twelve bytes. The record and unsigned combo-byte views below share
 * one backing object; neither is a separate pending action. */
typedef struct {
    s8 result;      /* 0x00 */
    s8 field1;      /* 0x01 */
    /* Cleared beside result and field1 by Ai_SelectFusionPlay's second hit block,
     * the only writer this record has for it. */
    s8 field_02;    /* 0x02 */
    char pad_03[3]; /* 0x03 */
    s8 value;       /* 0x06 */
    /* AiScript_LoadGuardianStarChoice stores the low byte of an AI register
     * here; DuelScene_UpdateCardMove reads bit 0 as bytes[7]. */
    s8 zero;        /* 0x07 */
    u8 face_down;   /* 0x08 */
    /* The field-action source and target codes. DuelScene_UpdateFieldActions
     * reads them as bytes[9] and bytes[10]; AiScript_PlayFieldCard writes all three.
     * The source is an own-field slot code: 1..5 for the monster row and
     * 6..10 for the back row, decoded as (a - 1) % 5 and a >= 6. The target
     * is a mixed code: values below 0x38 are own slots in the same encoding,
     * and 0x38 and above select the opposing monster at column 0x3C - a.
     *
     * change_position is the script's third AiScript_PlayFieldCard operand.
     * For a monster-row source, DuelScene_UpdateFieldActions takes a nonzero
     * value to its position step: it rotates the card with
     * DuelCard_UpdateDefenseRotation, turns it face up and ends the action
     * without attacking. A zero target and zero change_position only turn
     * the card face up; any other combination proceeds to the attack or
     * activation. */
    s8 source_code; /* 0x09 */
    s8 target_code; /* 0x0A */
    s8 change_position; /* 0x0B */
} AiSelectionFields;

typedef union {
    AiSelectionFields fields;
    u8 bytes[sizeof(AiSelectionFields)];
} AiSelection;

extern AiSelection gAi_Selection;

/* Byte 0x08 of that same selection -- AiSelection.fields.face_down -- is the
 * hand face-down request: Ai_SelectMagicPlay and Ai_SelectHandPlay set it to 1
 * or rand() & 1, and
 * the AI script opcodes set, clear and load it. Its loader zero-extends it
 * (lbu), so the field is unsigned.
 *
 * DuelScene_UpdateHandActions reads the same byte with %hi/%lo, and does so as
 * gAi_Selection.bytes[8]: the symbol-plus-offset form is what orders its %hi ahead of
 * the neighbouring constants. */

/* Byte 0x01 of the same selection -- AiSelection.fields.field1 -- has no
 * symbol of its own. Seven sites across three sources write the byte:
 * AiScript_PlayFaceUp (ai_script.c) stores the second of the five combo bytes as
 * gAi_Selection.bytes[1], duel_scene.c clears it the same way in
 * Duel_SelectEquipPair and Duel_SelectTrapPlay, and duel_scene.c writes
 * it as gAi_Selection.fields.field1.
 * DuelScene_UpdateHandActions is the only reader and reads the byte view too:
 * retail's `lui %hi` / `lbu %lo` pair links to the same bytes either way, and
 * the table form is the one that reproduces the instruction order. */

typedef char AiActiveCard_size_must_be_0x0C[
    sizeof(AiActiveCard) == AI_ACTIVE_CARD_RECORD_SIZE ? 1 : -1
];
typedef char AiActiveCard_card_id_offset_must_be_0x00[
    OFFSETOF(AiActiveCard, card_id) == 0x00 ? 1 : -1
];
typedef char AiActiveCard_attack_offset_must_be_0x02[
    OFFSETOF(AiActiveCard, attack) == 0x02 ? 1 : -1
];
typedef char AiActiveCard_defense_offset_must_be_0x04[
    OFFSETOF(AiActiveCard, defense) == 0x04 ? 1 : -1
];
typedef char AiActiveCard_flags_offset_must_be_0x06[
    OFFSETOF(AiActiveCard, flags) == 0x06 ? 1 : -1
];
typedef char AiActiveCard_card_type_offset_must_be_0x08[
    OFFSETOF(AiActiveCard, card_type) == 0x08 ? 1 : -1
];
typedef char AiActiveCard_guardian_star_offset_must_be_0x09[
    OFFSETOF(AiActiveCard, guardian_star) == 0x09 ? 1 : -1
];
typedef char AiActiveCard_guardian_star_2_offset_must_be_0x0A[
    OFFSETOF(AiActiveCard, guardian_star_2) == 0x0A ? 1 : -1
];
typedef char AiActiveCard_deck_index_offset_must_be_0x0B[
    OFFSETOF(AiActiveCard, deck_index) == 0x0B ? 1 : -1
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
    OFFSETOF(AiScriptState, card_set) == 0x3E ? 1 : -1
];
typedef char AiScriptState_card_set_entry_size_must_be_2[
    sizeof(((AiScriptState *)0)->card_set[0]) ==
        AI_SCRIPT_CARD_SET_ENTRY_SIZE ? 1 : -1
];
typedef char AiScriptState_type_set_offset_must_be_0x7E[
    OFFSETOF(AiScriptState, type_set) == 0x7E ? 1 : -1
];
typedef char AiScriptState_combo_offset_must_be_0x38[
    OFFSETOF(AiScriptState, combo_cards) == 0x38 ? 1 : -1
];
typedef char AiScriptState_unread_flag_offset_must_be_0x97[
    OFFSETOF(AiScriptState, unread_flag) == 0x97 ? 1 : -1
];
typedef char AiScriptState_attack_best_stat_offset_must_be_0x98[
    OFFSETOF(AiScriptState, attack_best_stat) == 0x98 ? 1 : -1
];
typedef char AiScriptState_attack_best_slot_offset_must_be_0x9A[
    OFFSETOF(AiScriptState, attack_best_slot) == 0x9A ? 1 : -1
];
typedef char AiScriptState_attack_best_target_offset_must_be_0x9B[
    OFFSETOF(AiScriptState, attack_best_target) == 0x9B ? 1 : -1
];
typedef char AiScriptState_fusion_count_offset_must_be_0x9C[
    OFFSETOF(AiScriptState, fusion_count) == 0x9C ? 1 : -1
];
typedef char AiScriptState_fusion_limit_offset_must_be_0x9D[
    OFFSETOF(AiScriptState, fusion_limit) == 0x9D ? 1 : -1
];
typedef char AiScriptState_fusion_set_offset_must_be_0x9E[
    OFFSETOF(AiScriptState, fusion_set) == 0x9E ? 1 : -1
];
typedef char AiScriptState_fusion_best_stat_offset_must_be_0xA0[
    OFFSETOF(AiScriptState, fusion_best_stat) == 0xA0 ? 1 : -1
];
typedef char AiScriptState_fusion_depth_offset_must_be_0xA2[
    OFFSETOF(AiScriptState, fusion_depth) == 0xA2 ? 1 : -1
];
typedef char AiScriptState_fusion_best_depth_offset_must_be_0xA3[
    OFFSETOF(AiScriptState, fusion_best_depth) == 0xA3 ? 1 : -1
];
typedef char AiScriptState_fusion_path_offset_must_be_0xA4[
    OFFSETOF(AiScriptState, fusion_path) == 0xA4 ? 1 : -1
];
typedef char AiScriptState_fusion_used_offset_must_be_0xAA[
    OFFSETOF(AiScriptState, fusion_used) == 0xAA ? 1 : -1
];
typedef char AiSelection_size_must_be_0x0C[
    sizeof(AiSelection) == 0x0C ? 1 : -1
];
typedef char AiSelectionFields_size_must_be_0x0C[
    sizeof(AiSelectionFields) == 0x0C ? 1 : -1
];
typedef char AiSelection_result_offset_must_be_0x00[
    OFFSETOF(AiSelectionFields, result) == 0x00 ? 1 : -1
];
typedef char AiSelection_field1_offset_must_be_0x01[
    OFFSETOF(AiSelectionFields, field1) == 0x01 ? 1 : -1
];
typedef char AiSelection_field_02_offset_must_be_0x02[
    OFFSETOF(AiSelectionFields, field_02) == 0x02 ? 1 : -1
];
typedef char AiSelection_value_offset_must_be_0x06[
    OFFSETOF(AiSelectionFields, value) == 0x06 ? 1 : -1
];
typedef char AiSelection_zero_offset_must_be_0x07[
    OFFSETOF(AiSelectionFields, zero) == 0x07 ? 1 : -1
];
typedef char AiSelection_face_down_offset_must_be_0x08[
    OFFSETOF(AiSelectionFields, face_down) == 0x08 ? 1 : -1
];
typedef char AiSelection_source_code_offset_must_be_0x09[
    OFFSETOF(AiSelectionFields, source_code) == 0x09 ? 1 : -1
];
typedef char AiSelection_target_code_offset_must_be_0x0A[
    OFFSETOF(AiSelectionFields, target_code) == 0x0A ? 1 : -1
];
typedef char AiSelection_change_position_offset_must_be_0x0B[
    OFFSETOF(AiSelectionFields, change_position) == 0x0B ? 1 : -1
];

extern s32 gAiScript_aMemory[AI_SCRIPT_MEMORY_COUNT];
extern AiScriptHandler gAiScript_apfnCommand[];
extern AiActiveCard gDuel_aActiveCards[];

void AiScript_Init(u8 *script);
s32 AiScript_Run(void);
s32 Ai_IsCardInSets(s32 mode, s32 index);
s32 Ai_GetHandSearchCount(void);
void Ai_GetWinningCardRange(s32 kind, s32 *low, s32 *high);
void Ai_GetCardRange(s32 kind, s32 *low, s32 *high);
/* The singular pair, next to the plural above and defined in the same unit:
 * each takes an index into gDuel_aActiveCards and asks whether that card's id,
 * or its type, appears in the script state's own set. Ai_IsTypeInSet compares
 * against type_set entries with AI_SCRIPT_TYPE_SET_ENCODING_BIAS removed,
 * which is why the two cannot share one implementation. */
s32 Ai_IsCardInSet(s32 index);
s32 Ai_IsTypeInSet(s32 index);
/* Recursive one-step-deeper search for the best fusion or equip starting from
 * `card_id`: walks the unused own-hand slots, and for a partner outside the
 * current fusion set takes Duel_CheckFusion, or Duel_CheckEquip when that
 * fails, then keeps the pairing whose result has the higher base attack --
 * preferring the shallower path when two tie. Returns nothing; the answer is
 * left in the script state's fusion_best_stat, fusion_best_depth and
 * combo_cards, and it returns immediately when fusion_count is zero. */
void Ai_CompleteFusion(s32 card_id);

/* AI script diagnostics. AiScript_Print passes the checkpoint string and its
 * format to printf (the format also receives the checkpoint byte); the
 * nest-overflow and return-underflow errors print with gAiScript_szSourceFile
 * through a "%s:%d\n" format. */
extern const char gAiScript_szCheckpoint[];
extern const char gAiScript_szCheckpointFormat[];
extern const char gAiScript_szNestOverflowError[];
extern const char gAiScript_szSourceFile[];
extern const char gAiScript_szReturnUnderflowError[];

#endif
