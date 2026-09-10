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

/* The record itself, as the two units that walk it typed spell it: both
 * func_80029590 and func_8002A3CC open with
 * `LibraryMotionState *state = &D_800EA1E8;` and work through the fields whose
 * offsets are asserted above.
 *
 * The same address is also read as `u8 D_800EA1E8[]` by func_8002BAB4.c and
 * func_8002BFCC.c, which take only the first byte -- func_8002BAB4.c's comment
 * calls it "the low nibble of D_800EA1E8's first byte" and dispatches the
 * library screen state on it. Neither of those files includes this header, so
 * the two views never meet and no guarded arm is needed here.
 *
 * That byte view is not folded in on purpose. Whether the mode byte is a field
 * of this record or a separate object sharing its first bytes is not
 * established, and a typed declaration reaching those two files would assert
 * that it is. func_8002A4A8 reaches the address through an asm .reloc, and two
 * further accessors are still assembly. */
extern LibraryMotionState D_800EA1E8;

s32 func_8002A3CC(void);

#endif
