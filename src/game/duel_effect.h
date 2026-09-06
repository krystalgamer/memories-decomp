#ifndef YUGIOH_GAME_DUEL_EFFECT_H
#define YUGIOH_GAME_DUEL_EFFECT_H

#include "../types.h"

#define DUEL_EFFECT_OFFSET(type, member) ((u32)&(((type *)0)->member))

#define DUEL_EFFECT_CHANNEL_COUNT 4
#define DUEL_EFFECT_ENTRY_COUNT 620
#define DUEL_EFFECT_OCCUPANCY_COUNT 240
#define DUEL_EVENT_SCRIPT_FLAG_DIALOG_ACTIVE 0x4000
#define DUEL_EVENT_SCRIPT_FLAG_STARTED 0x8000
#define TEXT_BOX_FLAG_BUILD_REQUESTED 0x800
#define TEXT_BOX_FLAG_DONE 0x2000

typedef struct {
    u8 pad_00[0x28];
    s32 field_28;
    s32 field_2C;
    void *field_30;
    u16 flags_34;
    u16 field_36;
    u16 field_38;
    u16 field_3A;
    s16 field_3C;
    s16 field_3E;
    s16 field_40;
    s16 field_42;
    u8 pad_44[0x0F];
    u8 field_53;
    u8 field_54;
    u8 pad_55[2];
    u8 index_57;
    u8 pad_58;
    u8 field_59;
    u8 field_5A;
    u8 field_5B;
    u16 range_start_5C;
    u16 range_count_5E;
    u8 field_60;
    u8 field_61;
    u8 pad_62[2];
} DuelEffectChannel;

typedef struct {
    u8 pad_00[0x11];
    u8 flags_11;
    u8 field_12;
    u8 field_13;
    u8 pad_14;
    u8 field_15;
    u8 pad_16[2];
    u8 field_18;
    u8 pad_19[3];
} DuelEffectEntry;

typedef char DuelEffectChannel_size_must_be_0x64[
    sizeof(DuelEffectChannel) == 0x64 ? 1 : -1
];
typedef char DuelEffectChannel_field_28_offset_must_be_0x28[
    DUEL_EFFECT_OFFSET(DuelEffectChannel, field_28) == 0x28 ? 1 : -1
];
typedef char DuelEffectChannel_field_30_offset_must_be_0x30[
    DUEL_EFFECT_OFFSET(DuelEffectChannel, field_30) == 0x30 ? 1 : -1
];
typedef char DuelEffectChannel_flags_34_offset_must_be_0x34[
    DUEL_EFFECT_OFFSET(DuelEffectChannel, flags_34) == 0x34 ? 1 : -1
];
typedef char DuelEffectChannel_field_3C_offset_must_be_0x3C[
    DUEL_EFFECT_OFFSET(DuelEffectChannel, field_3C) == 0x3C ? 1 : -1
];
typedef char DuelEffectChannel_field_53_offset_must_be_0x53[
    DUEL_EFFECT_OFFSET(DuelEffectChannel, field_53) == 0x53 ? 1 : -1
];
typedef char DuelEffectChannel_index_57_offset_must_be_0x57[
    DUEL_EFFECT_OFFSET(DuelEffectChannel, index_57) == 0x57 ? 1 : -1
];
typedef char DuelEffectChannel_field_5A_offset_must_be_0x5A[
    DUEL_EFFECT_OFFSET(DuelEffectChannel, field_5A) == 0x5A ? 1 : -1
];
typedef char DuelEffectChannel_range_start_5C_offset_must_be_0x5C[
    DUEL_EFFECT_OFFSET(DuelEffectChannel, range_start_5C) == 0x5C ? 1 : -1
];
typedef char DuelEffectChannel_field_61_offset_must_be_0x61[
    DUEL_EFFECT_OFFSET(DuelEffectChannel, field_61) == 0x61 ? 1 : -1
];

typedef char DuelEffectEntry_size_must_be_0x1C[
    sizeof(DuelEffectEntry) == 0x1C ? 1 : -1
];
typedef char DuelEffectEntry_flags_11_offset_must_be_0x11[
    DUEL_EFFECT_OFFSET(DuelEffectEntry, flags_11) == 0x11 ? 1 : -1
];
typedef char DuelEffectEntry_field_12_offset_must_be_0x12[
    DUEL_EFFECT_OFFSET(DuelEffectEntry, field_12) == 0x12 ? 1 : -1
];
typedef char DuelEffectEntry_field_13_offset_must_be_0x13[
    DUEL_EFFECT_OFFSET(DuelEffectEntry, field_13) == 0x13 ? 1 : -1
];
typedef char DuelEffectEntry_field_15_offset_must_be_0x15[
    DUEL_EFFECT_OFFSET(DuelEffectEntry, field_15) == 0x15 ? 1 : -1
];
typedef char DuelEffectEntry_field_18_offset_must_be_0x18[
    DUEL_EFFECT_OFFSET(DuelEffectEntry, field_18) == 0x18 ? 1 : -1
];

#undef DUEL_EFFECT_OFFSET

extern DuelEffectChannel D_800EB0F8[DUEL_EFFECT_CHANNEL_COUNT];
extern DuelEffectEntry D_800EB288[DUEL_EFFECT_ENTRY_COUNT];

#endif
