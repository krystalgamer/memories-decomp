#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_LIFECYCLE_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_LIFECYCLE_H

#include "../types.h"

/* Narrow lifecycle view for the initialization marker and uniform RGB fade. */
typedef struct {
    u8 pad_00[0x0C];
    u8 red;
    u8 green;
    u8 blue;
    u8 pad_0F[0x51];
    s16 fade_step;
    u8 pad_62[0x0A];
    u8 flags;
} DisplayObjectLifecycle;

#define DISPLAY_OBJECT_LIFECYCLE_OFFSET(member) \
    ((u32)&((DisplayObjectLifecycle *)0)->member)
typedef char DisplayObjectLifecycle_rgb_offsets_must_match[
    DISPLAY_OBJECT_LIFECYCLE_OFFSET(red) == 0x0C &&
    DISPLAY_OBJECT_LIFECYCLE_OFFSET(green) == 0x0D &&
    DISPLAY_OBJECT_LIFECYCLE_OFFSET(blue) == 0x0E ? 1 : -1
];
typedef char DisplayObjectLifecycle_fade_step_offset_must_be_0x60[
    DISPLAY_OBJECT_LIFECYCLE_OFFSET(fade_step) == 0x60 ? 1 : -1
];
typedef char DisplayObjectLifecycle_flags_offset_must_be_0x6C[
    DISPLAY_OBJECT_LIFECYCLE_OFFSET(flags) == 0x6C ? 1 : -1
];
#undef DISPLAY_OBJECT_LIFECYCLE_OFFSET

s32 DisplayObject_MarkInitialized(DisplayObjectLifecycle *object);
void DisplayObject_FadeBrightnessAndRelease(DisplayObjectLifecycle *object);

#endif
