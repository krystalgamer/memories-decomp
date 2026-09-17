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
 * &D_801A7AD8[5]. Duel_CollectMatchingFieldCardObjects retains this alias
 * for its two-side traversal and the original relocation.
 * Duel_CollectFieldRowCardObjects selects its row through D_801A7AD8
 * instead. */
extern DuelCardRecord D_801A7B64[];

/* Writes a zero-terminated list of the occupied cards' object addresses for
 * one whole row, with no filter. The row is the front row of the side
 * D_8009B1D5 does not select, or the row behind it when back_row is nonzero.
 * Six u32 words are always enough: five addresses plus the terminator. Both
 * traced overlay call sites pass a nonzero back_row, so the front row has no
 * traced caller; notes/duel-card-record.md records them. */
void Duel_CollectFieldRowCardObjects(u32 *output, s32 back_row);
/* Writes a zero-terminated list of occupied cards' object addresses.
 * Negative selectors scan both sides. Other selectors scan the front row of
 * the side D_8009B1D5 does not select: 0..20 match the object's 0x68 card
 * type byte, and 21+ require at least that attack value from the low half of
 * Duel_CalcCardStats.
 * The caller must provide at least 21 u32 words for a negative selector
 * (20 addresses plus the terminator), or six words otherwise (five plus the
 * terminator). There is no capacity argument. Both traced callers live in the
 * unsplit duel overlay and share one fixed output buffer at 0x8015B838, whose
 * 0x58 zero bytes of room match that 21-word worst case;
 * notes/duel-card-record.md records them. */
void Duel_CollectMatchingFieldCardObjects(u32 *output, s32 selector);

/* Packed per-card attribute word, indexed by card id minus one. Callers
 * unpack it with CARD_STAT_TYPE_SHIFT / CARD_STAT_TYPE_MASK for the card
 * type and CARD_STAT_VALUE_MASK for the stat value. */
extern s32 gDuel_adwCardStats[];

/* Level and attribute nibbles, indexed directly by the one-based card id. */
extern u8 gDuel_abCardLevelAttr[];

/* Signed name sort key, indexed by the same card id minus one: an ordering
 * over the card names that callers compare instead of the name text.
 * BuildDeck_CompareCard and func_80032BD4 in card_list_sort.c read it only as
 * the tie-break, when the two items' primary keys are equal. main_menu's
 * comparators read the same table for their whole ordering and substitute
 * 0x7FFFFFFF for id 0, which is where the empty slot sorting last comes from
 * -- that is those comparators' rule, not this table's.
 *
 * Declared here beside gDuel_adwCardStats because the two are read at the
 * same index by the callers that use both. It used to be a file-local
 * `extern s16 gCard_asNameSortKey[];` in card_list_sort.c -- a file that
 * already included this header for its twin -- and a second, address-named
 * declaration in src/overlays/main_menu/card_tables.h. */
extern s16 gCard_asNameSortKey[];

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
