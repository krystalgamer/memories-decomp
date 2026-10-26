#ifndef MEMORIES_DECOMP_JAPANESE_DUEL_EFFECT_CHANNEL_H
#define MEMORIES_DECOMP_JAPANESE_DUEL_EFFECT_CHANNEL_H

#include "../../types.h"

typedef struct {
    u8 pad_00[0x34];
    u16 flags_34;
    u16 initial_string_id;
    u16 pen_x;
    u16 pen_y;
    s16 rect_x;
    s16 rect_width;
    s16 rect_y;
    s16 rect_height;
    u8 pad_44[0x0F];
    u8 glyph_delay;
    u8 color_index_54;
    u8 pad_55[2];
    u8 channel_index;
    u8 pad_58;
    u8 depth_offset;
    u8 glyph_advance_x;
    u8 line_advance_y;
    u8 entry_range_start;
    u8 entry_range_count;
    u8 output_count;
    u8 output_limit;
} JapaneseDuelEffectChannel;

typedef char JapaneseDuelEffectChannel_size_must_be_0x60[
    sizeof(JapaneseDuelEffectChannel) == 0x60 ? 1 : -1
];

#endif
