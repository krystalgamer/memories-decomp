#ifndef MEMORIES_DECOMP_JAPANESE_DUEL_EFFECT_CHANNEL_H
#define MEMORIES_DECOMP_JAPANESE_DUEL_EFFECT_CHANNEL_H

#include "../../types.h"

typedef struct {
    u8 pad_00[0x34];
    u16 flags_34;
    u16 field_36;
    u16 pen_x;
    u16 pen_y;
    s16 rect_x;
    s16 rect_width;
    s16 rect_y;
    s16 rect_height;
    u8 pad_44[0x0F];
    u8 delay_reload_53;
    u8 color_index_54;
    u8 pad_55[2];
    u8 index_57;
    u8 pad_58;
    u8 field_59;
    u8 glyph_advance_x;
    u8 line_advance_y;
    u8 range_start_5C;
    u8 range_count_5D;
    u8 pad_5E;
    u8 field_5F;
} JapaneseDuelEffectChannel;

typedef char JapaneseDuelEffectChannel_size_must_be_0x60[
    sizeof(JapaneseDuelEffectChannel) == 0x60 ? 1 : -1
];

#endif
