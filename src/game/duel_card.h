#ifndef YUGIOH_GAME_DUEL_CARD_H
#define YUGIOH_GAME_DUEL_CARD_H

#include "../types.h"
#include "duel_card_layout.h"
#include "duel_grid.h"

#define DUEL_CARD_RECORD_OFFSET(type, member) ((u32)&(((type *)0)->member))

/* pad_08 is not unknown: func_80015DFC takes this same 0x1C record as
 * DisplayProjectionTrackedObject and writes its projected screen_x and
 * screen_y into 0x08 and 0x0A. func_800164FC walks D_801A7B64 as card
 * records and hands them straight to it, which is what ties the two
 * views together. The remaining disagreement is 0x18: table_index here,
 * s8 field_18 there, and func_80015DFC's `< 0xF` test needs the signed
 * reading -- so the views are not merged until that is settled. */
typedef struct {
    void *object;
    void *data;
    u8 pad_08[4];
    s16 card_id;
    s16 attack;
    s16 defense;
    s16 stat_modifier;
    s16 terrain_modifier;
    u16 flags;
    u8 table_index;
    u8 pad_19[3];
} DuelCardRecord;

typedef char DuelCardRecord_size_must_be_0x1C[
    sizeof(DuelCardRecord) == DUEL_CARD_RECORD_SIZE ? 1 : -1
];
typedef char DuelCardRecord_object_offset_must_be_0x00[
    DUEL_CARD_RECORD_OFFSET(DuelCardRecord, object) == 0x00 ? 1 : -1
];
typedef char DuelCardRecord_data_offset_must_be_0x04[
    DUEL_CARD_RECORD_OFFSET(DuelCardRecord, data) == 0x04 ? 1 : -1
];
typedef char DuelCardRecord_card_id_offset_must_be_0x0C[
    DUEL_CARD_RECORD_OFFSET(DuelCardRecord, card_id) == 0x0C ? 1 : -1
];
typedef char DuelCardRecord_attack_offset_must_be_0x0E[
    DUEL_CARD_RECORD_OFFSET(DuelCardRecord, attack) == 0x0E ? 1 : -1
];
typedef char DuelCardRecord_defense_offset_must_be_0x10[
    DUEL_CARD_RECORD_OFFSET(DuelCardRecord, defense) == 0x10 ? 1 : -1
];
typedef char DuelCardRecord_stat_modifier_offset_must_be_0x12[
    DUEL_CARD_RECORD_OFFSET(DuelCardRecord, stat_modifier) == 0x12 ? 1 : -1
];
typedef char DuelCardRecord_terrain_modifier_offset_must_be_0x14[
    DUEL_CARD_RECORD_OFFSET(DuelCardRecord, terrain_modifier) == 0x14 ? 1 : -1
];
typedef char DuelCardRecord_flags_offset_must_be_0x16[
    DUEL_CARD_RECORD_OFFSET(DuelCardRecord, flags) == 0x16 ? 1 : -1
];
typedef char DuelCardRecord_table_index_offset_must_be_0x18[
    DUEL_CARD_RECORD_OFFSET(DuelCardRecord, table_index) == 0x18 ? 1 : -1
];

#undef DUEL_CARD_RECORD_OFFSET

extern DuelCardRecord D_801A7AD8[];

/* The same table, entered five records in: 0x801A7B64 is 0x8C past
 * D_801A7AD8 and 0x8C is 5 * DUEL_CARD_RECORD_SIZE, so this is
 * &D_801A7AD8[5]. duel_card_object_queries.c already said so in its own
 * words -- "the records this file walks from it are the same 0x1C card
 * records the function above walks from D_801A7AD8". Both of its functions
 * are now candidates (src/candidates/func_8002C938.c and func_8002C9B4.c).
 *
 * The second name is load bearing and is not an inference here:
 * func_8002C9B4 reaches it from an asm block as
 *
 *     lui $2,%hi(D_801A7B64)
 *     addiu %0,$2,%lo(D_801A7B64)
 *
 * so the relocation names this symbol in the source itself. Rewriting it as
 * an offset from D_801A7AD8 would change what that block relocates against.
 */
extern DuelCardRecord D_801A7B64[];

/* Packed per-card attribute word, indexed by card id minus one. Callers
 * unpack it with CARD_STAT_TYPE_SHIFT / CARD_STAT_TYPE_MASK for the card
 * type and CARD_STAT_VALUE_MASK for the stat value. */
extern s32 gDuel_adwCardStats[];

/* Effective attack and defense packed into one word: defense in the high
 * half, attack in the low half. Never narrow the return type -- callers
 * select a half with `>> 16` or a 16-bit mask. */
s32 Duel_CalcCardStats(DuelCardRecord *card);

/* Guardian star matchup between two cards; 0 when `right` is null. */
s32 Duel_CalcGuardianStarBonus(DuelCardRecord *left, DuelCardRecord *right);

/* Battle stats for `card` against `opponent`, clamped to CARD_STAT_MAX:
 * the attack half of Duel_CalcCardStats plus the guardian star bonus, and
 * the defense half plus the same bonus. */
s32 Duel_CalcBattleAttack(DuelCardRecord *card, DuelCardRecord *opponent);
s32 Duel_CalcBattleDefense(DuelCardRecord *card, DuelCardRecord *opponent);

/* Clears three fields in every entry of the D_801A7AD8 card record table:
 * the word at +0x00, the word at +0x04 and the halfword at +0x16, which are
 * `object`, `data` and `flags` in the record above. It walks all
 * DUEL_CARD_RECORD_COUNT entries and touches nothing else, so it resets the
 * records rather than freeing or reinitialising them.
 *
 * Declared here because this header owns everything the walk is written in
 * terms of: D_801A7AD8 and DuelCardRecord above, and
 * DUEL_CARD_RECORD_COUNT from the duel_card_layout.h it includes. */
void func_8001778C(void);

#endif
