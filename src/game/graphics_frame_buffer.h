#ifndef YUGIOH_GAME_GRAPHICS_FRAME_BUFFER_H
#define YUGIOH_GAME_GRAPHICS_FRAME_BUFFER_H

#include "../types.h"
#include "../psyq/libgs.h"
#include "graphics_frame.h"

struct GraphicsFrameBuffer {
    u8 ordering_table_tags[0x5110];
    GsOT ordering_tables[4];
};

typedef char GraphicsFrameBuffer_ordering_tables_offset_must_be_0x5110[
    (u32)&((GraphicsFrameBuffer *)0)->ordering_tables == 0x5110 ? 1 : -1
];
typedef char GraphicsFrameBuffer_size_must_be_0x5160[
    sizeof(GraphicsFrameBuffer) == 0x5160 ? 1 : -1
];

#endif
