#ifndef MEMORIES_DECOMP_JAPANESE_DUEL_EFFECT_CHANNEL_H
#define MEMORIES_DECOMP_JAPANESE_DUEL_EFFECT_CHANNEL_H

#include "../../types.h"

typedef struct {
    u8 pad_00[0x34];
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
    u8 range_start_5C;
    u8 range_count_5D;
    u8 pad_5E;
    u8 field_5F;
} JapaneseDuelEffectChannel;

typedef char JapaneseDuelEffectChannel_size_must_be_0x60[
    sizeof(JapaneseDuelEffectChannel) == 0x60 ? 1 : -1
];

#endif
