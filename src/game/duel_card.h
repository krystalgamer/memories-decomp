#ifndef YUGIOH_GAME_DUEL_CARD_H
#define YUGIOH_GAME_DUEL_CARD_H

#include "../types.h"
#include "duel_grid.h"

#define DUEL_CARD_RECORD_OFFSET(type, member) ((u32)&(((type *)0)->member))

#define DUEL_CARD_RECORD_COUNT 30
#define DUEL_CARD_SIDE_RECORD_COUNT 15

typedef struct {
    void *object;
    void *data;
    u8 pad_08[4];
    u16 card_id;
    s16 attack;
    s16 defense;
    s16 stat_modifier;
    s16 terrain_modifier;
    u16 flags;
    u8 table_index;
    u8 pad_19[3];
} DuelCardRecord;

typedef char DuelCardRecord_size_must_be_0x1C[
    sizeof(DuelCardRecord) == 0x1C ? 1 : -1
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

#endif
