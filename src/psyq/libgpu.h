#ifndef MEMORIES_DECOMP_PSYQ_LIBGPU_H
#define MEMORIES_DECOMP_PSYQ_LIBGPU_H

#include "../types.h"

#define PSYQ_LIBGPU_OFFSET(type, member) ((u32)&(((type *)0)->member))

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} RECT;

typedef char RECT_size_must_be_8[sizeof(RECT) == 8 ? 1 : -1];
typedef char RECT_y_offset_must_be_2[
    PSYQ_LIBGPU_OFFSET(RECT, y) == 2 ? 1 : -1
];
typedef char RECT_w_offset_must_be_4[
    PSYQ_LIBGPU_OFFSET(RECT, w) == 4 ? 1 : -1
];
typedef char RECT_h_offset_must_be_6[
    PSYQ_LIBGPU_OFFSET(RECT, h) == 6 ? 1 : -1
];

#undef PSYQ_LIBGPU_OFFSET

#endif
