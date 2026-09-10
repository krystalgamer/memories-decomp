#ifndef MEMORIES_DECOMP_FUNC_8002A3CC_H
#define MEMORIES_DECOMP_FUNC_8002A3CC_H

#include "../types.h"
#include "display_object.h"

#define LIBRARY_MOTION_STATE_OFFSET(member) \
    ((u32)&(((LibraryMotionState *)0)->member))

typedef struct {
    u8 pad_00[8];
    s16 x;
    s16 y;
    u16 x_fraction;
    u16 y_fraction;
    u8 pad_10[2];
    u16 rest_x;
    u16 rest_y;
    u8 frames;
    u8 active;
    s32 velocity_x;
    s32 velocity_y;
    u8 pad_20[4];
    /* Eight display object pointers, 0x24 through 0x40, which func_80029590
       fills one per iteration. They were inside pad_20 until now; naming them
       moves nothing, and render still begins at 0x44 immediately after the
       last of them. */
    DisplayObject *slots[8];
    DisplayObject *render;
} LibraryMotionState;

typedef char LibraryMotionState_x_offset_must_be_0x8[
    LIBRARY_MOTION_STATE_OFFSET(x) == 0x8 ? 1 : -1
];
typedef char LibraryMotionState_frames_offset_must_be_0x16[
    LIBRARY_MOTION_STATE_OFFSET(frames) == 0x16 ? 1 : -1
];
typedef char LibraryMotionState_velocity_x_offset_must_be_0x18[
    LIBRARY_MOTION_STATE_OFFSET(velocity_x) == 0x18 ? 1 : -1
];
typedef char LibraryMotionState_render_offset_must_be_0x44[
    LIBRARY_MOTION_STATE_OFFSET(render) == 0x44 ? 1 : -1
];
typedef char LibraryMotionState_size_must_be_0x48[
    sizeof(LibraryMotionState) == 0x48 ? 1 : -1
];

#undef LIBRARY_MOTION_STATE_OFFSET

s32 func_8002A3CC(void);

#endif
